/*
 *  SPDDecomposeWaveforms.h
 *
 *  Created by Pete Bunting on 06/03/2012.
 *  Copyright 2012 SPDLib. All rights reserved.
 *
 *  This file is part of SPDLib.
 *
 *  SPDLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SPDLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SPDLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SPDDecomposeWaveforms_H
#define SPDDecomposeWaveforms_H

#include <iostream>
#include <string>
#include <list>

#include <boost/cstdint.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include "spd/SPDCommon.h"
#include "spd/SPDFile.h"
#include "spd/SPDPoint.h"
#include "spd/SPDPulse.h"
#include "spd/SPDProcessingException.h"
#include "spd/SPDDataBlockProcessor.h"
#include "spd/SPDProcessPulses.h"
#include "spd/SPDPulseProcessor.h"
#include "spd/SPDIOException.h"
#include "spd/SPDDataExporter.h"
#include "spd/SPDDataImporter.h"
#include "spd/SPDMathsUtils.h"
#include "spd/SPDDataExporter.h"
#include "spd/SPDDataImporter.h"
#include "spd/SPDFileReader.h"

#include "spd/cmpfit/mpfit.h"

using namespace std;
using namespace boost;
using boost::numeric_cast;
using boost::numeric::bad_numeric_cast;
using boost::numeric::positive_overflow;
using boost::numeric::negative_overflow;

namespace spdlib
{
    enum SPDDecompOption
    {
        spd_decomp_all,
        spd_decomp_indvid
    };
    
    class SPDDecomposeWaveforms
    {
    public:
        SPDDecomposeWaveforms();
        void decomposeWaveforms(string inFilePath, string outFilePath, boost::uint_fast32_t blockXSize, boost::uint_fast32_t blockYSize, SPDDecompOption decompOption, boost::uint_fast32_t intThreshold, bool thresholdSet, bool noiseSet, uint_fast32_t window, boost::uint_fast32_t decayThres, float decayVal) throw(SPDException);
        ~SPDDecomposeWaveforms();
    };
    
    class SPDDecomposePulse
    {
    public:
        SPDDecomposePulse(){};
        virtual void decompose(SPDPulse *pulse, SPDFile *spdFile) = 0;
        virtual ~SPDDecomposePulse(){};
    };
    
    class SPDDecomposePulseAll : public SPDDecomposePulse
    {
    public:
        SPDDecomposePulseAll(SPDInitDecomposition *findInitPts, boost::uint_fast32_t intThreshold, bool thresholdSet, bool noiseSet);
        void decompose(SPDPulse *pulse, SPDFile *spdFile) throw(SPDProcessingException);
        ~SPDDecomposePulseAll();
    protected:
        SPDInitDecomposition *findInitPts;
        boost::uint_fast32_t intThreshold;
        bool thresholdSet;
        bool noiseSet;
        mp_config *mpConfigValues;
		mp_result *mpResultsValues;
    };
    
    class SPDDecomposePulseIndividually : public SPDDecomposePulse
    {
    public:
        SPDDecomposePulseIndividually(SPDInitDecomposition *findInitPts, boost::uint_fast16_t waveFitWindow, boost::uint_fast32_t intThreshold, bool thresholdSet);
        void decompose(SPDPulse *pulse, SPDFile *spdFile) throw(SPDProcessingException);
        ~SPDDecomposePulseIndividually();
        SPDInitDecomposition *findInitPts;
        boost::uint_fast16_t waveFitWindow;
        boost::uint_fast32_t intThreshold;
        bool thresholdSet;
        mp_config *mpConfigValues;
		mp_result *mpResultsValues;
    };
    
    class SPDDecomposePulseImportProcessor : public SPDImporterProcessor
	{
	public:
		SPDDecomposePulseImportProcessor(SPDDecomposePulse *decompose, SPDDataExporter *exporter, SPDFile *spdFileOut) throw(SPDException)
        {
            this->decompose = decompose;
            this->exporter = exporter;
            this->spdFileOut = spdFileOut;
            
            if(exporter->requireGrid())
            {
                throw SPDException("This class does not support the export of gridded formats.");
            }
            
            try 
            {
                this->exporter->open(this->spdFileOut, this->spdFileOut->getFilePath());
            }
            catch (SPDException &e) 
            {
                throw e;
            }
            this->pulses = new list<SPDPulse*>();
        }
		void processImportedPulse(SPDFile *spdFile, SPDPulse *pulse) throw(SPDIOException)
        {
            try 
            {
                decompose->decompose(pulse, spdFile);
                this->pulses->push_back(pulse);
                this->exporter->writeDataColumn(pulses, 0, 0);
            }
            catch (SPDIOException &e) 
            {
                throw e;
            }
            catch (SPDException &e) 
            {
                throw SPDIOException(e.what());
            }
        };
		void completeFileAndClose(SPDFile *spdFile)throw(SPDIOException)
        {
            try 
            {
                spdFileOut->copyAttributesFrom(spdFile);
                spdFileOut->setDecomposedPtDefined(SPD_TRUE);
                spdFileOut->setDiscretePtDefined(SPD_TRUE);
                spdFileOut->setReceiveWaveformDefined(SPD_TRUE);
                exporter->finaliseClose();
            }
            catch (SPDIOException &e) 
            {
                throw e;
            }
        };
		~SPDDecomposePulseImportProcessor()
        {
            delete pulses;
        }
	private:
		SPDDecomposePulse *decompose;
		SPDDataExporter *exporter;
		SPDFile *spdFileOut;
        list<SPDPulse*> *pulses;
	};
    
    
    class SPDDecomposePulseColumnProcessor : public SPDPulseProcessor
	{
	public:
        SPDDecomposePulseColumnProcessor(SPDDecomposePulse *decompose)
        {
            this->decompose = decompose;
        }
        
        void processDataColumnImage(SPDFile *inSPDFile, vector<SPDPulse*> *pulses, float *imageData, SPDXYPoint *cenPts, boost::uint_fast32_t numImgBands, float binSize) throw(SPDProcessingException)
        {throw SPDProcessingException("Processing is not implemented for processDataColumnImage().");};
		void processDataColumn(SPDFile *inSPDFile, vector<SPDPulse*> *pulses, SPDXYPoint *cenPts) throw(SPDProcessingException)
        {
            try
            {
                if(pulses->size() > 0)
                {
                    for(vector<SPDPulse*>::iterator iterPulses = pulses->begin(); iterPulses != pulses->end(); ++iterPulses)
                    {
                        decompose->decompose((*iterPulses), inSPDFile);
                    }
                }
            }
            catch(SPDProcessingException &e)
            {
                throw e;
            }
        };
        void processDataWindowImage(SPDFile *inSPDFile, vector<SPDPulse*> ***pulses, float ***imageData, SPDXYPoint ***cenPts, boost::uint_fast32_t numImgBands, boost::uint_fast16_t winSize) throw(SPDProcessingException)
        {throw SPDProcessingException("Processing using a window is not implemented.");};
		void processDataWindow(SPDFile *inSPDFile, vector<SPDPulse*> ***pulses, SPDXYPoint ***cenPts, boost::uint_fast16_t winSize) throw(SPDProcessingException)
        {throw SPDProcessingException("Processing using a window is not implemented.");};
        
        vector<string> getImageBandDescriptions() throw(SPDProcessingException)
        {
            return vector<string>();
        };
        void setHeaderValues(SPDFile *spdFile) throw(SPDProcessingException)
        {
            spdFile->setDecomposedPtDefined(SPD_TRUE);
            spdFile->setDiscretePtDefined(SPD_TRUE);
            spdFile->setReceiveWaveformDefined(SPD_TRUE);
        };
        ~SPDDecomposePulseColumnProcessor()
        {
            
        };
    protected:
        SPDDecomposePulse *decompose;
	};
	

}

#endif



/*
 *  SPDCalcMetrics.h
 *  SPDLIB
 *
 *  Created by Pete Bunting on 17/03/2011.
 *  Copyright 2011 SPDLib. All rights reserved.
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
 */

#ifndef SPDCalcMetrics_H
#define SPDCalcMetrics_H

#include <iostream>
#include <string>
#include <list>
#include <vector>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include "spd/SPDFile.h"
#include "spd/SPDPoint.h"
#include "spd/SPDPulse.h"
#include "spd/SPDMetrics.h"
#include "spd/SPDTextFileUtilities.h"
#include "spd/SPDProcessPulses.h"
#include "spd/SPDPulseProcessor.h"
#include "spd/SPDProcessingException.h"
#include "spd/SPDPolygonProcessor.h"
#include "spd/SPDSetupProcessPolygons.h"
#include "spd/SPDProcessPolygons.h"

using namespace std;
using namespace xercesc;

namespace spdlib
{
	
	class SPDCalcMetrics
	{
	public:
		SPDCalcMetrics();
		void calcMetricToImage(string inXMLFilePath, string inputSPDFile, string outputImage, boost::uint_fast32_t blockXSize=250, boost::uint_fast32_t blockYSize=250, float processingResolution=0, string gdalFormat="ENVI") throw (SPDProcessingException);
        void calcMetricToVectorShp(string inXMLFilePath, string inputSPDFile, string inputVectorShp, string outputVectorShp, bool deleteOutShp, bool copyAttributes) throw (SPDProcessingException);
        void calcMetricForVector2ASCII(string inXMLFilePath, string inputSPDFile, string inputVectorShp, string outputASCII) throw (SPDProcessingException);
		~SPDCalcMetrics();
    protected:
        void parseMetricsXML(string inXMLFilePath, vector<SPDMetric*> *metrics, vector<string> *fieldNames) throw(SPDProcessingException);
        SPDMetric* createMetric(DOMElement *metricElement) throw(SPDProcessingException);
	};
    

    
    class SPDCalcPolyMetrics : public SPDPolygonProcessor
	{
	public:
		SPDCalcPolyMetrics(vector<SPDMetric*> *metrics, vector<string> *fieldNames);
		void processFeature(OGRFeature *inFeature, OGRFeature *outFeature,boost::uint_fast64_t fid, vector<SPDPulse*> *pulses, SPDFile *spdFile) throw(SPDProcessingException);
        void processFeature(OGRFeature *inFeature, ofstream *outASCIIFile, boost::uint_fast64_t fid, vector<SPDPulse*> *pulses, SPDFile *spdFile) throw(SPDProcessingException);
        void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(SPDProcessingException);
        void writeASCIIHeader(ofstream *outASCIIFile) throw(SPDProcessingException);
		~SPDCalcPolyMetrics();
    private:
        vector<SPDMetric*> *metrics;
        vector<string> *fieldNames;
	};
    
    
    class SPDCalcImageMetrics : public SPDPulseProcessor
	{
	public:
        SPDCalcImageMetrics(vector<SPDMetric*> *metrics, vector<string> *fieldNames);
        
        void processDataColumnImage(SPDFile *inSPDFile, vector<SPDPulse*> *pulses, float *imageData, SPDXYPoint *cenPts, boost::uint_fast32_t numImgBands, float binSize) throw(SPDProcessingException);

		void processDataColumn(SPDFile *inSPDFile, vector<SPDPulse*> *pulses, SPDXYPoint *cenPts) throw(SPDProcessingException)
        {throw SPDProcessingException("Processing must output an image. therefore function is not implemented.");};
        void processDataWindowImage(SPDFile *inSPDFile, vector<SPDPulse*> ***pulses, float ***imageData, SPDXYPoint ***cenPts, boost::uint_fast32_t numImgBands, boost::uint_fast16_t winSize) throw(SPDProcessingException)
        {throw SPDProcessingException("Processing using a window is not implemented.");};
		void processDataWindow(SPDFile *inSPDFile, vector<SPDPulse*> ***pulses, SPDXYPoint ***cenPts, boost::uint_fast16_t winSize) throw(SPDProcessingException)
        {throw SPDProcessingException("Processing using a window is not implemented.");};
        
        vector<string> getImageBandDescriptions() throw(SPDProcessingException);
        void setHeaderValues(SPDFile *spdFile) throw(SPDProcessingException);
        
        ~SPDCalcImageMetrics();
    private:
        vector<SPDMetric*> *metrics;
        vector<string> *fieldNames;
	};
}

#endif
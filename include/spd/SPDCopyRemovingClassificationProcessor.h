 /*
  *  SPDCopyRemovingClassificationProcessor.h
  *  SPDLIB
  *
  *  Created by Pete Bunting on 28/12/2010.
  *  Copyright 2010 SPDLib. All rights reserved.
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

#ifndef SPDCopyRemovingClassificationProcessor_H
#define SPDCopyRemovingClassificationProcessor_H

#include <iostream>
#include <string>
#include <vector>
#include "math.h"

#include <boost/math/special_functions/fpclassify.hpp>

#include "spd/SPDFile.h"
#include "spd/SPDPulse.h"
#include "spd/SPDPoint.h"
#include "spd/SPDProcessPulses.h"
#include "spd/SPDPulseProcessor.h"
#include "spd/SPDProcessingException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef libspd_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace spdlib
{

	class DllExport SPDCopyRemovingClassificationProcessor : public SPDPulseProcessor
	{
	public:
        SPDCopyRemovingClassificationProcessor();
        
        void processDataColumnImage(SPDFile *inSPDFile, std::vector<SPDPulse*> *pulses, float *imageData, SPDXYPoint *cenPts, boost::uint_fast32_t numImgBands, float binSize) 
        {throw SPDProcessingException("Processing is not implemented for processDataColumn().");};
        
		void processDataColumn(SPDFile *inSPDFile, std::vector<SPDPulse*> *pulses, SPDXYPoint *cenPts) ;
        
        void processDataWindowImage(SPDFile *inSPDFile, bool **validBins, std::vector<SPDPulse*> ***pulses, float ***imageData, SPDXYPoint ***cenPts, boost::uint_fast32_t numImgBands, float binSize, boost::uint_fast16_t winSize) 
        {throw SPDProcessingException("Processing using a window is not implemented.");};
		void processDataWindow(SPDFile *inSPDFile, bool **validBins, std::vector<SPDPulse*> ***pulses, SPDXYPoint ***cenPts, boost::uint_fast16_t winSize) 
        {throw SPDProcessingException("Processing using a window is not implemented.");};
        
        std::vector<std::string> getImageBandDescriptions() {return std::vector<std::string>();};
        void setHeaderValues(SPDFile *spdFile) {};
        
        ~SPDCopyRemovingClassificationProcessor();
	};

}

#endif




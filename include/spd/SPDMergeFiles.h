 /*
  *  SPDMergeFiles.h
  *  SPDLIB
  *
  *  Created by Pete Bunting on 19/12/2010.
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


#ifndef SPDMergeFiles_H
#define SPDMergeFiles_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "spd/SPDFile.h"
#include "spd/SPDPoint.h"
#include "spd/SPDPulse.h"

#include "spd/SPDDataImporter.h"
#include "spd/SPDDataExporter.h"
#include "spd/SPDIOFactory.h"
#include "spd/SPDExportAsReadUnGridded.h"

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
	class DllExport SPDMergeFiles
	{
	public:
		SPDMergeFiles();
		void mergeToUPD(std::vector<std::string> inputFiles, std::string output, std::string inFormat, std::string schema, std::string inSpatialRef, bool convertCoords, std::string outputProj4, boost::uint_fast16_t indexCoords, bool setSourceID, bool setReturnIDs, std::vector<boost::uint_fast16_t> returnID, bool setClasses, std::vector<boost::uint_fast16_t> classValues, bool ignoreChecks, boost::uint_fast16_t waveBinRes, bool keepMinExtent) ;
		~SPDMergeFiles();
	};
}

#endif


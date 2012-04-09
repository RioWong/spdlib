/*
 *  SPDConvertFormats.h
 *  spdlib_prj
 *
 *  Created by Pete Bunting on 13/10/2009.
 *  Copyright 2009 SPDLib. All rights reserved.
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


#ifndef SPDConvertFormats_H
#define SPDConvertFormats_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include "spd/SPDFile.h"
#include "spd/SPDPoint.h"
#include "spd/SPDPulse.h"

#include "spd/SPDGridData.h"
#include "spd/SPDDataImporter.h"
#include "spd/SPDDataExporter.h"
#include "spd/SPDIOUtils.h"
#include "spd/SPDIOFactory.h"
#include "spd/SPDFileIncrementalReader.h"
#include "spd/SPDExportAsReadUnGridded.h"
#include "spd/SPDExportAsTiles.h"

using namespace std;

namespace spdlib
{	
	class SPDConvertFormats
	{
	public:
		SPDConvertFormats();
		void convertInMemory(string input, string output, string inFormat, string schema, string outFormat, float binsize, string inSpatialRef, bool convertCoords, string outputProj4,boost::uint_fast16_t indexCoords, bool defineTL, double tlX, double tlY, bool defineOrigin, double originX, double originY, float originZ, bool useSphericIdx, bool usePolarIdx, bool useScanIdx, float waveNoiseThreshold,boost::uint_fast16_t waveformBitRes, boost::uint_fast16_t pointVersion, boost::uint_fast16_t pulseVersion) throw(SPDException);
		void convertToSPDUsingRowTiles(string input, string output, string inFormat, string schema, float binsize, string inSpatialRef, bool convertCoords, string outputProjWKT,boost::uint_fast16_t indexCoords, string tempdir,boost::uint_fast16_t numRowsInTile, bool defineTL, double tlX, double tlY, bool defineOrigin, double originX, double originY, float originZ, bool useSphericIdx, bool usePolarIdx, bool useScanIdx, float waveNoiseThreshold,boost::uint_fast16_t waveformBitRes, bool keepTmpFiles, boost::uint_fast16_t pointVersion, boost::uint_fast16_t pulseVersion) throw(SPDException);
		void convertToSPDUsingBlockTiles(string input, string output, string inFormat, string schema, float binsize, string inSpatialRef, bool convertCoords, string outputProjWKT,boost::uint_fast16_t indexCoords, string tempdir,boost::uint_fast16_t numRowsInTile, boost::uint_fast16_t numColsInTile, bool defineTL, double tlX, double tlY, bool defineOrigin, double originX, double originY, float originZ, bool useSphericIdx, bool usePolarIdx, bool useScanIdx, float waveNoiseThreshold,boost::uint_fast16_t waveformBitRes, bool keepTmpFiles, boost::uint_fast16_t pointVersion, boost::uint_fast16_t pulseVersion) throw(SPDException);
		void copySPD2SPD(SPDFile *inSPDFile, SPDFile *outSPDFile) throw(SPDException);
        ~SPDConvertFormats();
	};
}

#endif

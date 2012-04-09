/*
 *  SPDASCIIMultiLineReader.cpp
 *  SPDLIB
 *
 *  Created by Pete Bunting on 28/04/2011.
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


#ifndef SPDASCIIMultiLineReader_H
#define SPDASCIIMultiLineReader_H

#include <list>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include <boost/cstdint.hpp>

#include "ogrsf_frmts.h"

#include "spd/SPDFile.h"
#include "spd/SPDPulse.h"
#include "spd/SPDIOException.h"
#include "spd/SPDTextFileLineReader.h"
#include "spd/SPDTextFileUtilities.h"
#include "spd/SPDDataImporter.h"

using namespace std;
using namespace boost;

namespace spdlib
{
	
	class SPDASCIIMultiLineReader : public SPDDataImporter
	{
	public:
		SPDASCIIMultiLineReader(bool convertCoords=false, string outputProjWKT="", string schema="", boost::uint_fast16_t indexCoords=SPD_FIRST_RETURN, bool defineOrigin=false, double originX=0, double originY=0, float originZ=0, float waveNoiseThreshold=0);
		SPDDataImporter* getInstance(bool convertCoords, string outputProjWKT, string schema, boost::uint_fast16_t indexCoords, bool defineOrigin, double originX, double originY, float originZ, float waveNoiseThreshold);
        list<SPDPulse*>* readAllDataToList(string, SPDFile *spdFile)throw(SPDIOException);
		vector<SPDPulse*>* readAllDataToVector(string inputFile, SPDFile *spdFile)throw(SPDIOException);
		void readAndProcessAllData(string inputFile, SPDFile *spdFile, SPDImporterProcessor *processor) throw(SPDIOException);
		bool isFileType(string fileType);
        void readHeaderInfo(string inputFile, SPDFile *spdFile) throw(SPDIOException);
		~SPDASCIIMultiLineReader();
	private:
        SPDPoint* convertLineToPoint(vector<string> *lineTokens)throw(SPDIOException);
		bool classWarningGiven;
	};
}

#endif



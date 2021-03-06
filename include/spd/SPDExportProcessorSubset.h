 /*
  *  SPDExportProcessorSubset.h
  *  SPDLIB
  *
  *  Created by Pete Bunting on 19/12/2010.
  *  Copyright 2010 RSGISLib. All rights reserved.
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

#ifndef SPDExportProcessorSubset_H
#define SPDExportProcessorSubset_H

#include <list>

#include <boost/cstdint.hpp>

#include "ogrsf_frmts.h"

#include "spd/SPDFile.h"
#include "spd/SPDPulse.h"
#include "spd/SPDIOException.h"
#include "spd/SPDDataExporter.h"
#include "spd/SPDFileWriter.h"
#include "spd/SPDFileReader.h"

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
	class DllExport SPDExportProcessorSubset : public SPDImporterProcessor
	{
	public:
		SPDExportProcessorSubset(SPDDataExporter *exporter, SPDFile *spdFileOut, double *bbox) ;
		void processImportedPulse(SPDFile *spdFile, SPDPulse *pulse) ;
		void completeFileAndClose(SPDFile *spdFile);
		~SPDExportProcessorSubset();
	private:
		SPDDataExporter *exporter;
		SPDFile *spdFileOut;
		bool fileOpen;
		std::list<SPDPulse*> *pulses;
		double *bbox;
		double xMin;
		double xMax;
		double yMin;
		double yMax;
		double zMin;
		double zMax;
		bool first;
	};

	class DllExport SPDExportProcessorSubsetSpherical : public SPDImporterProcessor
	{
	public:
		SPDExportProcessorSubsetSpherical(SPDDataExporter *exporter, SPDFile *spdFileOut, double *bbox) ;
		void processImportedPulse(SPDFile *spdFile, SPDPulse *pulse) ;
		void completeFileAndClose(SPDFile *spdFile);
		~SPDExportProcessorSubsetSpherical();
	private:
		SPDDataExporter *exporter;
		SPDFile *spdFileOut;
		bool fileOpen;
		std::list<SPDPulse*> *pulses;
		double *bbox;
	};

	class DllExport SPDExportProcessorSubsetScan : public SPDImporterProcessor
	{
	public:
		SPDExportProcessorSubsetScan(SPDDataExporter *exporter, SPDFile *spdFileOut, double *bbox) ;
		void processImportedPulse(SPDFile *spdFile, SPDPulse *pulse) ;
		void completeFileAndClose(SPDFile *spdFile);
		~SPDExportProcessorSubsetScan();
	private:
		SPDDataExporter *exporter;
		SPDFile *spdFileOut;
		bool fileOpen;
		std::list<SPDPulse*> *pulses;
		double *bbox;
	};
    
	class DllExport SPDSubsetNonGriddedFile
	{
	public:
		SPDSubsetNonGriddedFile();
		void subsetCartesian(std::string input, std::string output, double *bbox, bool *bboxDefined) ;
        void subsetSpherical(std::string input, std::string output, double *bbox, bool *bboxDefined) ;
        void subsetScan(std::string input, std::string output, double *bbox, bool *bboxDefined) ;
		~SPDSubsetNonGriddedFile();
	};
}

#endif




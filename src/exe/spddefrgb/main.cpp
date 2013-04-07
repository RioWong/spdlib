/*
 *  main.cpp
 *  spdlib
 *
 *  Created by Pete Bunting on 30/11/2010.
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
 *
 */

#include <string>
#include <iostream>
#include <algorithm>

#include <spd/tclap/CmdLine.h>

#include "spd/SPDTextFileUtilities.h"
#include "spd/SPDException.h"

#include "spd/SPDDefineRGBValues.h"

#include "spd/spd-config.h"

int main (int argc, char * const argv[]) 
{
    std::cout << "spddefrgb " << SPDLIB_PACKAGE_STRING << ", Copyright (C) " << SPDLIB_COPYRIGHT_YEAR << " Sorted Pulse Library (SPD)\n";
	std::cout << "This program comes with ABSOLUTELY NO WARRANTY. This is free software,\n";
	std::cout << "and you are welcome to redistribute it under certain conditions; See\n";
	std::cout << "website (http://www.spdlib.org). Bugs are to be reported on the trac\n";
	std::cout << "or directly to " << SPDLIB_PACKAGE_BUGREPORT << std::endl;
	
	try 
	{
        TCLAP::CmdLine cmd("Define the RGB values on the SPDFile: spddefrgb", ' ', "1.0.0");
		
        TCLAP::ValueArg<boost::uint_fast32_t> numOfRowsBlockArg("r","blockrows","Number of rows within a block (Default 100)",false,100,"unsigned int");
		cmd.add( numOfRowsBlockArg );
        
        TCLAP::ValueArg<boost::uint_fast32_t> numOfColsBlockArg("c","blockcols","Number of columns within a block (Default 0) - Note values greater than 1 result in a non-sequencial SPD file.",false,0,"unsigned int");
		cmd.add( numOfColsBlockArg );
		
		TCLAP::ValueArg<uint_fast16_t> redBandArg("","red","Image band for red channel",false,1,"uint_fast16_t");
		cmd.add( redBandArg );
		
		TCLAP::ValueArg<uint_fast16_t> greenBandArg("","green","Image band for green channel",false,2,"uint_fast16_t");
		cmd.add( greenBandArg );
		
		TCLAP::ValueArg<uint_fast16_t> blueBandArg("","blue","Image band for blue channel",false,3,"uint_fast16_t");
		cmd.add( blueBandArg );
		
		TCLAP::UnlabeledMultiArg<std::string> multiFileNames("Files", "File names for the input and output files (Image: Input SPD, Input Image, Output SPD)", true, "string");
		cmd.add( multiFileNames );
		cmd.parse( argc, argv );
		
		std::vector<std::string> fileNames = multiFileNames.getValue();		
		if(fileNames.size() != 3)
		{
			for(unsigned int i = 0; i < fileNames.size(); ++i)
			{
				std::cout << i << ": " << fileNames.at(i) << std::endl;
			}
			
            spdlib::SPDTextFileUtilities textUtils;
			std::string message = std::string("Three file paths should have been specified (e.g., Input and Output). ") + textUtils.uInt32bittostring(fileNames.size()) + std::string(" were provided.");
			throw spdlib::SPDException(message);
		}
		
        std::string inputSPDFile = fileNames.at(0);
   		std::string inputImageFile = fileNames.at(1);
		std::string outputSPDFile = fileNames.at(2);

		uint_fast16_t redBand = redBandArg.getValue()-1;	
        uint_fast16_t greenBand = greenBandArg.getValue()-1;	
        uint_fast16_t blueBand = blueBandArg.getValue()-1;
        
        spdlib::SPDFile *spdInFile = new spdlib::SPDFile(inputSPDFile);
        spdlib::SPDPulseProcessor *pulseProcessor = new spdlib::SPDDefineRGBValues(redBand, greenBand, blueBand);            
        spdlib::SPDSetupProcessPulses processPulses = spdlib::SPDSetupProcessPulses(numOfColsBlockArg.getValue(), numOfRowsBlockArg.getValue(), true);
        processPulses.processPulsesWithInputImage(pulseProcessor, spdInFile, outputSPDFile, inputImageFile);
        
        delete spdInFile;
        delete pulseProcessor;
	}
	catch (TCLAP::ArgException &e) 
	{
		std::cerr << "Parse Error: " << e.what() << std::endl;
	}
	catch(spdlib::SPDException &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}


/*
 *  main.cpp
 *  spdlib
 *
 *  Created by Pete Bunting on 15/01/2014.
 *  Copyright 2014 SPDLib. All rights reserved.
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
#include <stdlib.h>
#include <time.h>


#include "spd/SPDException.h"
#include "spd/SPDFile.h"
#include "spd/SPDPoint.h"
#include "spd/SPDPulse.h"
#include "spd/SPDProcessingException.h"
#include "spd/SPDDataBlockProcessor.h"
#include "spd/SPDProcessDataBlocks.h"

class SPDDataBlockProcessorRandomRGBVals : public spdlib::SPDDataBlockProcessor
{
public:
    SPDDataBlockProcessorRandomRGBVals(): spdlib::SPDDataBlockProcessor()
    {
        srand(time(NULL));
    };
    void processDataBlockImage(spdlib::SPDFile *inSPDFile, std::vector<spdlib::SPDPulse*> ***pulses, float ***imageDataBlock, spdlib::SPDXYPoint ***cenPts, boost::uint_fast32_t xSize, boost::uint_fast32_t ySize, boost::uint_fast32_t numImgBands, float binSize) throw(spdlib::SPDProcessingException)
    {
        throw spdlib::SPDProcessingException("Not implemented as not required.");
    };
    
    void processDataBlock(spdlib::SPDFile *inSPDFile, std::vector<spdlib::SPDPulse*> ***pulses, spdlib::SPDXYPoint ***cenPts, boost::uint_fast32_t xSize, boost::uint_fast32_t ySize, float binSize) throw(spdlib::SPDProcessingException)
    {
        // Loop through the pulses are give a random RGB value for each point between 0 - 255.
        for(boost::uint_fast32_t i = 0; i < ySize; ++i)
        {
            for(boost::uint_fast32_t j = 0; j < xSize; ++j)
            {
                for(std::vector<spdlib::SPDPulse*>::iterator iterPls = pulses[i][j]->begin(); iterPls != pulses[i][j]->end(); ++iterPls)
                {
                    if((*iterPls)->numberOfReturns > 0)
                    {
                        for(std::vector<spdlib::SPDPoint*>::iterator iterPts = (*iterPls)->pts->begin(); iterPts != (*iterPls)->pts->end(); ++iterPts)
                        {
                            (*iterPts)->red = rand() % 255 + 1;
                            (*iterPts)->green = rand() % 255 + 1;
                            (*iterPts)->blue = rand() % 255 + 1;
                        }
                    }
                }
            }
        }
    };
    
    void processDataBlockImage(spdlib::SPDFile *inSPDFile, std::vector<spdlib::SPDPulse*> *pulses, float ***imageDataBlock, spdlib::SPDXYPoint ***cenPts, boost::uint_fast32_t xSize, boost::uint_fast32_t ySize, boost::uint_fast32_t numImgBands) throw(spdlib::SPDProcessingException)
    {
        throw spdlib::SPDProcessingException("Not implemented as not required.");
    };
    
    void processDataBlock(spdlib::SPDFile *inSPDFile, std::vector<spdlib::SPDPulse*> *pulses) throw(spdlib::SPDProcessingException)
    {
        throw spdlib::SPDProcessingException("Not implemented as not required.");
    };
    
    std::vector<std::string> getImageBandDescriptions() throw(spdlib::SPDProcessingException)
    {
        // There is no image output so band names are just returned as a empty vector.
        return std::vector<std::string>();
    };
    
    void setHeaderValues(spdlib::SPDFile *spdFile) throw(spdlib::SPDProcessingException)
    {
        // There are no header values within the spd file to altered.
    };
    
    ~SPDDataBlockProcessorRandomRGBVals()
    {
        // There is nothing for the deconstructor to do...
    };
};


int main (int argc, char * const argv[])
{
    try
    {
        std::string inputSPDFile = "/Users/pete/Desktop/Wellington/als_dr_opt3_row44col32_2013_nztm_10m_rnm_pmfmccgrd_h_rgb.spd";
        std::string outputSPDFile = "/Users/pete/Desktop/Wellington/als_dr_opt3_row44col32_2013_nztm_10m_rnm_pmfmccgrd_h_norgb.spd";
        boost::uint_fast32_t overlap=1;
        boost::uint_fast32_t blockXSize=10;
        boost::uint_fast32_t blockYSize=10;
        bool printProgress=true;
        bool keepMinExtent=true;
        float processingResolution = 0; // If 0 then processing will be at the native bin size of the SPD file.
        
        spdlib::SPDFile *spdInFile = new spdlib::SPDFile(inputSPDFile);
        spdlib::SPDDataBlockProcessor *blockProcessor = new SPDDataBlockProcessorRandomRGBVals();
        spdlib::SPDProcessDataBlocks processBlocks = spdlib::SPDProcessDataBlocks(blockProcessor, overlap, blockXSize, blockYSize, printProgress, keepMinExtent);
        processBlocks.processDataBlocksGridPulsesOutputSPD(spdInFile, outputSPDFile, processingResolution);
        
        delete blockProcessor;
        delete spdInFile;
    }
    catch (spdlib::SPDException &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
    

}
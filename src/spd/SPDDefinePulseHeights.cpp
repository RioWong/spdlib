/*
 *  SPDDefinePulseHeights.cpp
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

#include "spd/SPDDefinePulseHeights.h"

namespace spdlib
{

    SPDDefinePulseHeights::SPDDefinePulseHeights(SPDPointInterpolator *interpolator) : SPDDataBlockProcessor()
    {
        this->interpolator = interpolator;
    }
        
    void SPDDefinePulseHeights::processDataBlockImage(SPDFile *inSPDFile, std::vector<SPDPulse*> ***pulses, float ***imageDataBlock, SPDXYPoint ***cenPts, boost::uint_fast32_t xSize, boost::uint_fast32_t ySize, boost::uint_fast32_t numImgBands, float binSize) 
    {
        if(numImgBands == 0)
        {
            throw SPDProcessingException("The input image needs to have at least 1 image band.");
        }
        
        int feedback = ySize/10.0;
        int feedbackCounter = 0;
        std::cout << "Started" << std::flush;
        for(boost::uint_fast32_t i = 0; i < ySize; ++i)
        {
            if(ySize < 10)
            {
                std::cout << "." << i << "." << std::flush;
            }
            else if((feedback != 0) && ((i % feedback) == 0))
            {
                std::cout << "." << feedbackCounter << "." << std::flush;
                feedbackCounter = feedbackCounter + 10;
            }
            
            for(boost::uint_fast32_t j = 0; j < xSize; ++j)
            {
                for(std::vector<SPDPulse*>::iterator iterPulses = pulses[i][j]->begin(); iterPulses != pulses[i][j]->end(); ++iterPulses)
                {
                    (*iterPulses)->h0 = (*iterPulses)->z0 - imageDataBlock[i][j][0];
                    if((*iterPulses)->numberOfReturns > 0)
                    {
                        for(std::vector<SPDPoint*>::iterator iterPts = (*iterPulses)->pts->begin(); iterPts != (*iterPulses)->pts->end(); ++iterPts)
                        {
                            (*iterPts)->height = (*iterPts)->z - imageDataBlock[i][j][0];
                        }
                    }
                }
            }
        }
        std::cout << " Complete.\n";
    }
		
    void SPDDefinePulseHeights::processDataBlock(SPDFile *inSPDFile, std::vector<SPDPulse*> ***pulses, SPDXYPoint ***cenPts, boost::uint_fast32_t xSize, boost::uint_fast32_t ySize, float binSize) 
    {
        try 
		{
            bool ptsAvail = true;
            try 
            {
                interpolator->initInterpolator(pulses, xSize, ySize, SPD_GROUND);
            }
            catch (SPDException &e) 
            {
                ptsAvail = false;
            }
            
            if(ptsAvail)
            {
                double elevVal = 0;
                
                int feedback = ySize/10.0;
                int feedbackCounter = 0;
                std::cout << "Started" << std::flush;
                
                for(boost::uint_fast32_t i = 0; i < ySize; ++i)
                {
                    if(ySize < 10)
                    {
                        std::cout << "." << i << "." << std::flush;
                    }
                    else if((feedback != 0) && ((i % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    for(boost::uint_fast32_t j = 0; j < xSize; ++j)
                    {
                        for(std::vector<SPDPulse*>::iterator iterPulses = pulses[i][j]->begin(); iterPulses != pulses[i][j]->end(); ++iterPulses)
                        {
                            elevVal = interpolator->getValue((*iterPulses)->x0, (*iterPulses)->y0);
                            (*iterPulses)->h0 = (*iterPulses)->z0 - elevVal;
                            if((*iterPulses)->numberOfReturns > 0)
                            {
                                for(std::vector<SPDPoint*>::iterator iterPts = (*iterPulses)->pts->begin(); iterPts != (*iterPulses)->pts->end(); ++iterPts)
                                {
                                    elevVal = interpolator->getValue((*iterPts)->x, (*iterPts)->y);
                                    (*iterPts)->height = (*iterPts)->z - elevVal;
                                }
                            }
                        }
                    }
                }
                std::cout << " Complete.\n";
            }
            
            interpolator->resetInterpolator();
        }
        catch (SPDProcessingException &e) 
        {
            throw e;
        }
    }
             
    SPDDefinePulseHeights::~SPDDefinePulseHeights()
    {
        
    }
    
}







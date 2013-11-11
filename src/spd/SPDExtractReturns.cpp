/*
 *  SPDExtractReturns.cpp
 *  SPDLIB
 *
 *  Created by Pete Bunting on 15/11/2012.
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

#include "spd/SPDExtractReturns.h"

namespace spdlib
{

    SPDExtractReturnsImportProcess::SPDExtractReturnsImportProcess(std::string outputFilePath, bool classValSet, boost::uint_fast16_t classID, bool returnValSet, boost::uint_fast16_t returnVal) throw(SPDException):SPDImporterProcessor()
    {
        this->classValSet = classValSet;
        this->classID = classID;
        this->returnValSet = returnValSet;
        this->returnVal = returnVal;
        
        this->outSPDFile = new SPDFile(outputFilePath);
        this->exporter = new SPDNoIdxFileWriter();
        
        this->exporter->open(this->outSPDFile, outputFilePath);
        
        this->pulses = new std::vector<SPDPulse*>();
    }
    
    void SPDExtractReturnsImportProcess::processImportedPulse(SPDFile *spdFile, SPDPulse *pulse) throw(SPDIOException)
    {
        if(pulse->pts->size() > 0)
        {
            if(returnValSet && (returnVal != SPD_ALL_RETURNS))
            {
                if(returnVal == SPD_FIRST_RETURNS)
                {
                    bool first = true;
                    for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                    {
                        if(first)
                        {
                            first = false;
                            ++iterPts;
                        }
                        else
                        {
                            delete *iterPts;
                            pulse->pts->erase(iterPts);
                        }
                    }
                }
                else if(returnVal == SPD_LAST_RETURNS)
                {
                    for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                    {
                        if(pulse->pts->size() > 1)
                        {
                            delete *iterPts;
                            pulse->pts->erase(iterPts);                            
                        }
                        else
                        {
                            ++iterPts;
                        }
                    }
                }
                else if(returnVal == SPD_NOTFIRST_RETURNS)
                {
                    bool first = true;
                    for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                    {
                        if(first)
                        {
                            delete *iterPts;
                            pulse->pts->erase(iterPts);
                        }
                        else
                        {
                            first = false;
                            ++iterPts;
                        }
                    }
                }
                else if(returnVal == SPD_FIRSTLAST_RETURNS)
                {
                    bool first = true;
                    for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                    {
                        if(first)
                        {
                            first = false;
                            ++iterPts;
                        }
                        else if(pulse->pts->size() > 2)
                        {
                            delete *iterPts;
                            pulse->pts->erase(iterPts);
                        }
                        else
                        {
                            ++iterPts;
                        }
                    }
                }
                else
                {
                    for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                    {
                        if((*iterPts)->returnID != returnVal)
                        {
                            delete *iterPts;
                            pulse->pts->erase(iterPts);
                        }
                        else
                        {
                            ++iterPts;
                        }
                    }
                }
            }
            
            if(classValSet && (classID != SPD_ALL_CLASSES))
            {
                if(classID == SPD_ALL_CLASSES_TOP)
                {
                    bool top = true;
                    for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                    {
                        if(top)
                        {
                            top = false;
                            ++iterPts;
                        }
                        else
                        {
                            delete *iterPts;
                            pulse->pts->erase(iterPts);
                        }
                    }
                }
                else if(classID == SPD_VEGETATION_TOP)
                {
                    bool top = true;
                    for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                    {
                        if(!top & (((*iterPts)->classification != SPD_LOW_VEGETATION) | ((*iterPts)->classification != SPD_MEDIUM_VEGETATION) |
                           ((*iterPts)->classification != SPD_HIGH_VEGETATION) | ((*iterPts)->classification != SPD_TRUNK) |
                           ((*iterPts)->classification != SPD_FOILAGE) | ((*iterPts)->classification != SPD_BRANCH)))
                        {
                            delete *iterPts;
                            pulse->pts->erase(iterPts);
                        }
                        else
                        {
                            top = false;
                            ++iterPts;
                        }
                    }
                }
                else if(classID == SPD_VEGETATION)
                {
                    for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                    {
                        if(((*iterPts)->classification != SPD_LOW_VEGETATION) | ((*iterPts)->classification != SPD_MEDIUM_VEGETATION) |
                           ((*iterPts)->classification != SPD_HIGH_VEGETATION) | ((*iterPts)->classification != SPD_TRUNK) |
                           ((*iterPts)->classification != SPD_FOILAGE) | ((*iterPts)->classification != SPD_BRANCH))
                        {
                            delete *iterPts;
                            pulse->pts->erase(iterPts);
                        }
                        else
                        {
                            ++iterPts;
                        }
                    }
                }
                else if(classID == SPD_NOT_GROUND)
                {
                    for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                    {
                        if((*iterPts)->classification == SPD_GROUND)
                        {
                            delete *iterPts;
                            pulse->pts->erase(iterPts);
                        }
                        else
                        {
                            ++iterPts;
                        }
                    }
                }
                else
                {
                    for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                    {
                        if((*iterPts)->classification != classID)
                        {
                            delete *iterPts;
                            pulse->pts->erase(iterPts);
                        }
                        else
                        {
                            ++iterPts;
                        }
                    }
                }
            }
            
            
        }
        
        pulse->numberOfReturns = pulse->pts->size();
        pulses->push_back(pulse);
        this->exporter->writeDataColumn(pulses, 0, 0);
        pulses->clear();
    }
    
    void SPDExtractReturnsImportProcess::completeFileAndClose(SPDFile *spdFile)throw(SPDIOException)
    {
        try
		{
            this->outSPDFile->copyAttributesFrom(spdFile);
			this->exporter->finaliseClose();
            delete pulses;
		}
		catch (SPDIOException &e)
		{
			throw e;
		}
    }
		
    SPDExtractReturnsImportProcess::~SPDExtractReturnsImportProcess()
    {
        
    }
    
    
    
    
    SPDExtractReturnsBlockProcess::SPDExtractReturnsBlockProcess(bool classValSet, boost::uint_fast16_t classID, bool returnValSet, boost::uint_fast16_t returnVal)
    {
        this->classValSet = classValSet;
        this->classID = classID;
        this->returnValSet = returnValSet;
        this->returnVal = returnVal;
    }
    
    void SPDExtractReturnsBlockProcess::processDataColumn(SPDFile *inSPDFile, std::vector<SPDPulse*> *pulses, SPDXYPoint *cenPts) throw(SPDProcessingException)
    {
        try
        {            
            if(pulses->size() > 0)
            {
                SPDPulse *pulse = NULL;
                for(std::vector<SPDPulse*>::iterator iterPulse = pulses->begin(); iterPulse != pulses->end(); ++iterPulse)
                {
                    pulse = *iterPulse;
                    if(returnValSet && (returnVal != SPD_ALL_RETURNS))
                    {
                        if(returnVal == SPD_FIRST_RETURNS)
                        {
                            bool first = true;
                            for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                            {
                                if(first)
                                {
                                    first = false;
                                    ++iterPts;
                                }
                                else
                                {
                                    delete *iterPts;
                                    pulse->pts->erase(iterPts);
                                }
                            }
                        }
                        else if(returnVal == SPD_LAST_RETURNS)
                        {
                            for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                            {
                                if(pulse->pts->size() > 1)
                                {
                                    delete *iterPts;
                                    pulse->pts->erase(iterPts);
                                }
                                else
                                {
                                    ++iterPts;
                                }
                            }
                        }
                        else if(returnVal == SPD_NOTFIRST_RETURNS)
                        {
                            bool first = true;
                            for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                            {
                                if(first)
                                {
                                    delete *iterPts;
                                    pulse->pts->erase(iterPts);
                                }
                                else
                                {
                                    first = false;
                                    ++iterPts;
                                }
                            }
                        }
                        else if(returnVal == SPD_FIRSTLAST_RETURNS)
                        {
                            bool first = true;
                            for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                            {
                                if(first)
                                {
                                    first = false;
                                    ++iterPts;
                                }
                                else if(pulse->pts->size() > 2)
                                {
                                    delete *iterPts;
                                    pulse->pts->erase(iterPts);
                                }
                                else
                                {
                                    ++iterPts;
                                }
                            }
                        }
                        else
                        {
                            for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                            {
                                if((*iterPts)->returnID != returnVal)
                                {
                                    delete *iterPts;
                                    pulse->pts->erase(iterPts);
                                }
                                else
                                {
                                    ++iterPts;
                                }
                            }
                        }
                    }
                    
                    if(classValSet && (classID != SPD_ALL_CLASSES))
                    {
                        if(classID == SPD_ALL_CLASSES_TOP)
                        {
                            bool top = true;
                            for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                            {
                                if(top)
                                {
                                    top = false;
                                    ++iterPts;
                                }
                                else
                                {
                                    delete *iterPts;
                                    pulse->pts->erase(iterPts);
                                }
                            }
                        }
                        else if(classID == SPD_VEGETATION_TOP)
                        {
                            bool top = true;
                            for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                            {
                                if(!top & (((*iterPts)->classification != SPD_LOW_VEGETATION) | ((*iterPts)->classification != SPD_MEDIUM_VEGETATION) |
                                           ((*iterPts)->classification != SPD_HIGH_VEGETATION) | ((*iterPts)->classification != SPD_TRUNK) |
                                           ((*iterPts)->classification != SPD_FOILAGE) | ((*iterPts)->classification != SPD_BRANCH)))
                                {
                                    delete *iterPts;
                                    pulse->pts->erase(iterPts);
                                }
                                else
                                {
                                    top = false;
                                    ++iterPts;
                                }
                            }
                        }
                        else if(classID == SPD_VEGETATION)
                        {
                            for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                            {
                                if(((*iterPts)->classification != SPD_LOW_VEGETATION) | ((*iterPts)->classification != SPD_MEDIUM_VEGETATION) |
                                   ((*iterPts)->classification != SPD_HIGH_VEGETATION) | ((*iterPts)->classification != SPD_TRUNK) |
                                   ((*iterPts)->classification != SPD_FOILAGE) | ((*iterPts)->classification != SPD_BRANCH))
                                {
                                    delete *iterPts;
                                    pulse->pts->erase(iterPts);
                                }
                                else
                                {
                                    ++iterPts;
                                }
                            }
                        }
                        else if(classID == SPD_NOT_GROUND)
                        {
                            for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                            {
                                if((*iterPts)->classification == SPD_GROUND)
                                {
                                    delete *iterPts;
                                    pulse->pts->erase(iterPts);
                                }
                                else
                                {
                                    ++iterPts;
                                }
                            }
                        }
                        else
                        {
                            for(std::vector<SPDPoint*>::iterator iterPts = pulse->pts->begin(); iterPts != pulse->pts->end(); )
                            {
                                if((*iterPts)->classification != classID)
                                {
                                    delete *iterPts;
                                    pulse->pts->erase(iterPts);
                                }
                                else
                                {
                                    ++iterPts;
                                }
                            }
                        }
                    }
                    
                    pulse->numberOfReturns = pulse->pts->size();
                }
            }            
        }
        catch (SPDProcessingException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw SPDProcessingException(e.what());
        }
        
    }
    
    SPDExtractReturnsBlockProcess::~SPDExtractReturnsBlockProcess()
    {
        
    }
    
}


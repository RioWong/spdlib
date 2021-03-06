/*
 *  SPDTextFileImporter.cpp
 *  spdlib
 *
 *  Created by Pete Bunting on 28/11/2010.
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

#include "spd/SPDTextFileImporter.h"


namespace spdlib
{

	SPDTextFileImporter::SPDTextFileImporter(SPDTextLineProcessor *lineParser, bool convertCoords, std::string outputProj4, std::string schema, boost::uint_fast16_t indexCoords, bool defineOrigin, double originX, double originY, float originZ, float waveNoiseThreshold): SPDDataImporter(convertCoords, outputProj4, schema, indexCoords, defineOrigin, originX, originY, originZ, waveNoiseThreshold), lineParser(NULL)
	{
		this->lineParser = lineParser;
	}
	
	SPDTextFileImporter::SPDTextFileImporter(const SPDTextFileImporter &textFileImporter): SPDDataImporter(textFileImporter), lineParser(NULL)
	{
		this->lineParser = textFileImporter.lineParser;
	}
    
    SPDDataImporter* SPDTextFileImporter::getInstance(bool convertCoords, std::string outputProjWKT, std::string schema, boost::uint_fast16_t indexCoords, bool defineOrigin, double originX, double originY, float originZ, float waveNoiseThreshold)
    {
        return new SPDTextFileImporter(this->lineParser, convertCoords, outputProjWKT, schema, indexCoords, defineOrigin, originX, originY, originZ, waveNoiseThreshold);
    }
		
	std::list<SPDPulse*>* SPDTextFileImporter::readAllDataToList(std::string inputFile, SPDFile *spdFile)
	{
		if(convertCoords)
		{
			throw SPDIOException("Coordinate Convertion is not implemented for this importer.");
		}
		SPDTextFileUtilities textUtils;
		SPDPulseUtils pulseUtils;
		std::list<SPDPulse*> *allPulses = new std::list<SPDPulse*>();
		boost::uint_fast64_t numPulses = 0;
		boost::uint_fast64_t numPoints = 0;
		
		boost::uint_fast64_t numOfLines = 0;
		try 
		{
			numOfLines = textUtils.countLines(inputFile);
		}
		catch (SPDIOException &e) 
		{
			throw e;
		}
		
		std::cout << "Number of Lines = " << numOfLines << std::endl;
		boost::uint_fast64_t feedback = numOfLines/10;
		int feedbackCount = 0;
		
		double minX = 0;
		double maxX = 0;
		double minY = 0;
		double maxY = 0;
		double minZ = 0;
		double maxZ = 0;
        double maxAzimuth = 0;
        double minAzimuth = 0;
        double maxZenith = 0;
        double minZenith = 0;
        double maxRange = 0;
        double minRange = 0;
		bool first = true;
        bool firstSph = true;
		
		std::cout << "Started (Read Data) ." << std::flush;
		
		try 
		{
			std::ifstream inputFileStream;
			inputFileStream.open(inputFile.c_str(), std::ios_base::in);
			if(inputFileStream.is_open())
			{
				lineParser->reset();
				std::string strLine = "";
				bool lineEnding = false;
				char ch = ' ';
				char lastch = ' ';
				inputFileStream.get(ch);
				while (!inputFileStream.eof()) 
				{					
					if ((ch == 0x0a) && (lastch == 0x0d))
					{
						lineEnding = true; // Windows Line Ending
					}
					else if ((lastch == 0x0d) && (ch != 0x0a)) 
					{
						lineEnding = true; // Mac Line Ending
					} 
					else if (ch == 0x0a) 
					{
						lineEnding = true; // UNIX Line Ending
					}
					
					if(lineEnding)
					{
						if(numPulses > 0)
                        {
                            if((numOfLines > 10) & ((numPulses % feedback) == 0))
                            {
                                std::cout << "." << feedbackCount << "." << std::flush;
                                feedbackCount += 10;
                            }
                        }
						
						try 
						{
                            boost::algorithm::trim(strLine);
							if(!lineParser->haveReadheader())
							{
								lineParser->parseHeader(strLine);
							}
							else 
							{
								SPDPulse *pl = new SPDPulse();
								pulseUtils.initSPDPulse(pl);
								
								if(lineParser->parseLine(strLine, pl, indexCoords))
								{
									pl->pulseID = numPulses++;
                                    
                                    if(defineOrigin)
                                    {
                                        pl->x0 = this->originX;
                                        pl->y0 = this->originY;
                                        pl->z0 = this->originZ;
                                        
                                        double zenith = 0;
                                        double azimuth = 0;
                                        double range = 0;
                                        
                                        for(std::vector<SPDPoint*>::iterator iterPts = pl->pts->begin(); iterPts != pl->pts->end(); ++iterPts)
                                        {
                                            SPDConvertToSpherical(pl->x0, pl->y0, pl->z0, (*iterPts)->x, (*iterPts)->y, (*iterPts)->z, &zenith, &azimuth, &range);
                                            (*iterPts)->range = range;
                                            if(firstSph)
                                            {
                                                maxAzimuth = azimuth;
                                                minAzimuth = azimuth;
                                                maxZenith = zenith;
                                                minZenith = zenith;
                                                maxRange = range;
                                                minRange = range;
                                                firstSph = false;
                                            }
                                            else
                                            {
                                                if(azimuth < minAzimuth)
                                                {
                                                    minAzimuth = azimuth;
                                                }
                                                else if(azimuth > maxAzimuth)
                                                {
                                                    maxAzimuth = azimuth;
                                                }
                                                
                                                if(zenith < minZenith)
                                                {
                                                    minZenith = zenith;
                                                }
                                                else if(zenith > maxZenith)
                                                {
                                                    maxZenith = zenith;
                                                }
                                                
                                                if(range < minRange)
                                                {
                                                    minRange = range;
                                                }
                                                else if(range > maxRange)
                                                {
                                                    maxRange = range;
                                                }
                                            }
                                        }
                                        
                                        pl->zenith = zenith;
                                        pl->azimuth = azimuth;
                                        
                                    }
									allPulses->push_back(pl);
									numPoints += pl->numberOfReturns;
									
									for(boost::uint_fast8_t i = 0; i < pl->numberOfReturns; ++i)
									{
										if(first)
										{
											minX = pl->pts->at(i)->x;
											maxX = pl->pts->at(i)->x;
											minY = pl->pts->at(i)->y;
											maxY = pl->pts->at(i)->y;
											minZ = pl->pts->at(i)->z;
											maxZ = pl->pts->at(i)->z;
											first = false;
										}
										else
										{
											if(pl->pts->at(i)->x < minX)
											{
												minX = pl->pts->at(i)->x;
											}
											else if(pl->pts->at(i)->x > maxX)
											{
												maxX = pl->pts->at(i)->x;
											}
											
											if(pl->pts->at(i)->y < minY)
											{
												minY = pl->pts->at(i)->y;
											}
											else if(pl->pts->at(i)->y > maxY)
											{
												maxY = pl->pts->at(i)->y;
											}
											
											if(pl->pts->at(i)->z < minZ)
											{
												minZ = pl->pts->at(i)->z;
											}
											else if(pl->pts->at(i)->z > maxZ)
											{
												maxZ = pl->pts->at(i)->z;
											}
										}
									}
								}
								else 
								{
									pulseUtils.deleteSPDPulse(pl);
								}
							}
							
						}
						catch (SPDIOException &e) 
						{
							inputFileStream.close();
							throw e;
						}
						
						strLine = "";
						lineEnding = false;
					}
					else 
					{
						strLine += ch;
					}
					
					lastch = ch;
					inputFileStream.get(ch);      
				}
				inputFileStream.close();
				
				lineParser->saveHeaderValues(spdFile);
			}
			else
			{
				std::string message = std::string("Text file ") + inputFile + std::string(" could not be openned.");
				throw SPDIOException(message.c_str());
			}
			
		}
		catch (SPDIOException &e) 
		{
			throw e;
		}
		std::cout << ".Complete\n";
		
		spdFile->setNumberOfPoints(numPoints);
		spdFile->setNumberOfPulses(numPulses);
		spdFile->setBoundingVolume(minX, maxX, minY, maxY, minZ, maxZ);
        spdFile->setBoundingVolumeSpherical(minZenith, maxZenith, minAzimuth, maxAzimuth, minRange, maxRange);
		
		return allPulses;
	}
	
	std::vector<SPDPulse*>* SPDTextFileImporter::readAllDataToVector(std::string inputFile, SPDFile *spdFile)
	{
		if(convertCoords)
		{
			throw SPDIOException("Coordinate Convertion is not implmented for this importer.");
		}
		SPDTextFileUtilities textUtils;
		SPDPulseUtils pulseUtils;
		std::vector<SPDPulse*> *allPulses = new std::vector<SPDPulse*>();
		boost::uint_fast64_t numPulses = 0;
		boost::uint_fast64_t numPoints = 0;
		
		boost::uint_fast64_t numOfLines = 0;
		try 
		{
			numOfLines = textUtils.countLines(inputFile);
		}
		catch (SPDIOException &e) 
		{
			throw e;
		}
		
		allPulses->reserve(numOfLines);
		
		std::cout << "Number of Lines = " << numOfLines << std::endl;
		boost::uint_fast64_t feedback = numOfLines/10;
		int feedbackCount = 0;
		
		double minX = 0;
		double maxX = 0;
		double minY = 0;
		double maxY = 0;
		double minZ = 0;
		double maxZ = 0;
        double maxAzimuth = 0;
        double minAzimuth = 0;
        double maxZenith = 0;
        double minZenith = 0;
        double maxRange = 0;
        double minRange = 0;
		bool first = true;
        bool firstSph = true;
		
		std::cout << "Started (Read Data) ." << std::flush;
		
		try 
		{
			std::ifstream inputFileStream;
			inputFileStream.open(inputFile.c_str(), std::ios_base::in);
			if(inputFileStream.is_open())
			{
				lineParser->reset();
				std::string strLine = "";
				bool lineEnding = false;
				char ch = ' ';
				char lastch = ' ';
				inputFileStream.get(ch);
				while (!inputFileStream.eof()) 
				{					
					if ((ch == 0x0a) && (lastch == 0x0d))
					{
						lineEnding = true; // Windows Line Ending
					}
					else if ((lastch == 0x0d) && (ch != 0x0a)) 
					{
						lineEnding = true; // Mac Line Ending
					} 
					else if (ch == 0x0a) 
					{
						lineEnding = true; // UNIX Line Ending
					}
					
					if(lineEnding)
					{
						if(numPulses > 0)
                        {
                            if((numOfLines > 10) & ((numPulses % feedback) == 0))
                            {
                                std::cout << "." << feedbackCount << "." << std::flush;
                                feedbackCount += 10;
                            }
                        }
						
						try 
						{
                            boost::algorithm::trim(strLine);
							if(!lineParser->haveReadheader())
							{
								lineParser->parseHeader(strLine);
							}
							else 
							{
								SPDPulse *pl = new SPDPulse();
								pulseUtils.initSPDPulse(pl);
								
								if(lineParser->parseLine(strLine, pl, indexCoords))
								{
									pl->pulseID = numPulses++;
                                    if(defineOrigin)
                                    {
                                        pl->x0 = this->originX;
                                        pl->y0 = this->originY;
                                        pl->z0 = this->originZ;
                                        
                                        double zenith = 0;
                                        double azimuth = 0;
                                        double range = 0;
                                        
                                        for(std::vector<SPDPoint*>::iterator iterPts = pl->pts->begin(); iterPts != pl->pts->end(); ++iterPts)
                                        {
                                            SPDConvertToSpherical(pl->x0, pl->y0, pl->z0, (*iterPts)->x, (*iterPts)->y, (*iterPts)->z, &zenith, &azimuth, &range);
                                            (*iterPts)->range = range;
                                            if(firstSph)
                                            {
                                                maxAzimuth = azimuth;
                                                minAzimuth = azimuth;
                                                maxZenith = zenith;
                                                minZenith = zenith;
                                                maxRange = range;
                                                minRange = range;
                                                firstSph = false;
                                            }
                                            else
                                            {
                                                if(azimuth < minAzimuth)
                                                {
                                                    minAzimuth = azimuth;
                                                }
                                                else if(azimuth > maxAzimuth)
                                                {
                                                    maxAzimuth = azimuth;
                                                }
                                                
                                                if(zenith < minZenith)
                                                {
                                                    minZenith = zenith;
                                                }
                                                else if(zenith > maxZenith)
                                                {
                                                    maxZenith = zenith;
                                                }
                                                
                                                if(range < minRange)
                                                {
                                                    minRange = range;
                                                }
                                                else if(range > maxRange)
                                                {
                                                    maxRange = range;
                                                }
                                            }
                                        }
                                        
                                        pl->zenith = zenith;
                                        pl->azimuth = azimuth;
                                    }
									allPulses->push_back(pl);
									numPoints += pl->numberOfReturns;
									
									for(boost::uint_fast8_t i = 0; i < pl->numberOfReturns; ++i)
									{
										if(first)
										{
											minX = pl->pts->at(i)->x;
											maxX = pl->pts->at(i)->x;
											minY = pl->pts->at(i)->y;
											maxY = pl->pts->at(i)->y;
											minZ = pl->pts->at(i)->z;
											maxZ = pl->pts->at(i)->z;
											first = false;
										}
										else
										{
											if(pl->pts->at(i)->x < minX)
											{
												minX = pl->pts->at(i)->x;
											}
											else if(pl->pts->at(i)->x > maxX)
											{
												maxX = pl->pts->at(i)->x;
											}
											
											if(pl->pts->at(i)->y < minY)
											{
												minY = pl->pts->at(i)->y;
											}
											else if(pl->pts->at(i)->y > maxY)
											{
												maxY = pl->pts->at(i)->y;
											}
											
											if(pl->pts->at(i)->z < minZ)
											{
												minZ = pl->pts->at(i)->z;
											}
											else if(pl->pts->at(i)->z > maxZ)
											{
												maxZ = pl->pts->at(i)->z;
											}
										}
									}
								}
								else 
								{
									pulseUtils.deleteSPDPulse(pl);
								}
							}
							
						}
						catch (SPDIOException &e) 
						{
							inputFileStream.close();
							throw e;
						}
						
						strLine = "";
						lineEnding = false;
					}
					else 
					{
						strLine += ch;
					}
					
					lastch = ch;
					inputFileStream.get(ch);      
				}
				inputFileStream.close();
				
				lineParser->saveHeaderValues(spdFile);
			}
			else
			{
				std::string message = std::string("Text file ") + inputFile + std::string(" could not be openned.");
				throw SPDIOException(message.c_str());
			}
			
		}
		catch (SPDIOException &e) 
		{
			throw e;
		}
		std::cout << ".Complete\n";
		
		spdFile->setNumberOfPoints(numPoints);
		spdFile->setNumberOfPulses(numPulses);
		spdFile->setBoundingVolume(minX, maxX, minY, maxY, minZ, maxZ);
        spdFile->setBoundingVolumeSpherical(minZenith, maxZenith, minAzimuth, maxAzimuth, minRange, maxRange);
		
		return allPulses;
	}
	
	void SPDTextFileImporter::readAndProcessAllData(std::string inputFile, SPDFile *spdFile, SPDImporterProcessor *processor)
	{        
		if(convertCoords)
		{
			throw SPDIOException("Coordinate Convertion is not implmented for this importer.");
		}
		SPDTextFileUtilities textUtils;
		SPDPulseUtils pulseUtils;
		boost::uint_fast64_t numPulses = 0;
		boost::uint_fast64_t numPoints = 0;
		
		boost::uint_fast64_t numOfLines = 0;
		try 
		{
			numOfLines = textUtils.countLines(inputFile);
		}
		catch (SPDIOException &e) 
		{
			throw e;
		}
		
		std::cout << "Number of Lines = " << numOfLines << std::endl;
		boost::uint_fast64_t feedback = numOfLines/10;
		int feedbackCount = 0;
		
		double minX = 0;
		double maxX = 0;
		double minY = 0;
		double maxY = 0;
		double minZ = 0;
		double maxZ = 0;
        double maxAzimuth = 0;
        double minAzimuth = 0;
        double maxZenith = 0;
        double minZenith = 0;
        double maxRange = 0;
        double minRange = 0;
		bool first = true;
        bool firstSph = true;
		
		std::cout << "Started (Read Data) ." << std::flush;
		
		try 
		{
			std::ifstream inputFileStream;
			inputFileStream.open(inputFile.c_str(), std::ios_base::in);
			if(inputFileStream.is_open())
			{
				lineParser->reset();
				std::string strLine = "";
				bool lineEnding = false;
				char ch = ' ';
				char lastch = ' ';
				inputFileStream.get(ch);
				while (!inputFileStream.eof()) 
				{					
					if ((ch == 0x0a) && (lastch == 0x0d))
					{
						lineEnding = true; // Windows Line Ending
					}
					else if ((lastch == 0x0d) && (ch != 0x0a)) 
					{
						lineEnding = true; // Mac Line Ending
					} 
					else if (ch == 0x0a) 
					{
						lineEnding = true; // UNIX Line Ending
					}
					
					if(lineEnding)
					{
						if(numPulses > 0)
                        {
                            if((numOfLines > 10) & ((numPulses % feedback) == 0))
                            {
                                std::cout << "." << feedbackCount << "." << std::flush;
                                feedbackCount += 10;
                            }
                        }
						
						try 
						{
							boost::algorithm::trim(strLine);
							if(!lineParser->haveReadheader())
							{
								lineParser->parseHeader(strLine);
							}
							else 
							{
								SPDPulse *pl = new SPDPulse();
								pulseUtils.initSPDPulse(pl);
								if(lineParser->parseLine(strLine, pl, indexCoords))
								{
									pl->pulseID = numPulses++;
                                    if(defineOrigin)
                                    {
                                        pl->x0 = this->originX;
                                        pl->y0 = this->originY;
                                        pl->z0 = this->originZ;
                                        
                                        double zenith = 0;
                                        double azimuth = 0;
                                        double range = 0;
                                        
                                        for(std::vector<SPDPoint*>::iterator iterPts = pl->pts->begin(); iterPts != pl->pts->end(); ++iterPts)
                                        {
                                            SPDConvertToSpherical(pl->x0, pl->y0, pl->z0, (*iterPts)->x, (*iterPts)->y, (*iterPts)->z, &zenith, &azimuth, &range);
                                            (*iterPts)->range = range;
                                            if(firstSph)
                                            {
                                                maxAzimuth = azimuth;
                                                minAzimuth = azimuth;
                                                maxZenith = zenith;
                                                minZenith = zenith;
                                                maxRange = range;
                                                minRange = range;
                                                firstSph = false;
                                            }
                                            else
                                            {
                                                if(azimuth < minAzimuth)
                                                {
                                                    minAzimuth = azimuth;
                                                }
                                                else if(azimuth > maxAzimuth)
                                                {
                                                    maxAzimuth = azimuth;
                                                }
                                                
                                                if(zenith < minZenith)
                                                {
                                                    minZenith = zenith;
                                                }
                                                else if(zenith > maxZenith)
                                                {
                                                    maxZenith = zenith;
                                                }
                                                
                                                if(range < minRange)
                                                {
                                                    minRange = range;
                                                }
                                                else if(range > maxRange)
                                                {
                                                    maxRange = range;
                                                }
                                            }
                                        }
                                        
                                        pl->zenith = zenith;
                                        pl->azimuth = azimuth;
                                    }
									numPoints += pl->numberOfReturns;
									for(boost::uint_fast16_t i = 0; i < pl->numberOfReturns; ++i)
									{
										if(first)
										{
											minX = pl->pts->at(i)->x;
											maxX = pl->pts->at(i)->x;
											minY = pl->pts->at(i)->y;
											maxY = pl->pts->at(i)->y;
											minZ = pl->pts->at(i)->z;
											maxZ = pl->pts->at(i)->z;
											first = false;
										}
										else
										{
											if(pl->pts->at(i)->x < minX)
											{
												minX = pl->pts->at(i)->x;
											}
											else if(pl->pts->at(i)->x > maxX)
											{
												maxX = pl->pts->at(i)->x;
											}
											
											if(pl->pts->at(i)->y < minY)
											{
												minY = pl->pts->at(i)->y;
											}
											else if(pl->pts->at(i)->y > maxY)
											{
												maxY = pl->pts->at(i)->y;
											}
											
											if(pl->pts->at(i)->z < minZ)
											{
												minZ = pl->pts->at(i)->z;
											}
											else if(pl->pts->at(i)->z > maxZ)
											{
												maxZ = pl->pts->at(i)->z;
											}
										}
									}
									processor->processImportedPulse(spdFile, pl);
								}
								else 
								{
									pulseUtils.deleteSPDPulse(pl);
								}								
							}
							
						}
						catch (SPDIOException &e) 
						{
							inputFileStream.close();
							throw e;
						}
						
						strLine = "";
						lineEnding = false;
					}
					else 
					{
						strLine += ch;
					}
					
					lastch = ch;
					inputFileStream.get(ch);      
				}
				inputFileStream.close();
				
				lineParser->saveHeaderValues(spdFile);
			}
			else
			{
				std::string message = std::string("Text file ") + inputFile + std::string(" could not be openned.");
				throw SPDIOException(message.c_str());
			}
			
		}
		catch (SPDIOException &e) 
		{
			throw e;
		}
		std::cout << ".Complete\n";
		
		spdFile->setNumberOfPoints(numPoints);
		spdFile->setNumberOfPulses(numPulses);
		spdFile->setBoundingVolume(minX, maxX, minY, maxY, minZ, maxZ);
        spdFile->setBoundingVolumeSpherical(minZenith, maxZenith, minAzimuth, maxAzimuth, minRange, maxRange);
	}
	
	bool SPDTextFileImporter::isFileType(std::string fileType)
	{
		return lineParser->isFileType(fileType);
	}
    
    void SPDTextFileImporter::readHeaderInfo(std::string, SPDFile*) 
    {
        // No Header to Read..
    }
    
    void SPDTextFileImporter::readSchema()
    {
        try 
        {
            this->lineParser->parseSchema(this->schema);
        } 
        catch (SPDIOException &e) 
        {
            throw e;
        }
    }
	
	SPDTextFileImporter& SPDTextFileImporter::operator=(const SPDTextFileImporter& textFileImporter)
	{
		this->lineParser = textFileImporter.lineParser;
		return *this;
	}
	
	SPDTextFileImporter::~SPDTextFileImporter()
	{
		delete lineParser;
	}
}



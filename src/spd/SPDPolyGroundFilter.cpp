/*
 *  SPDPolyGroundFilter.cpp
 *
 *  Created by Pete Bunting on 04/03/2012.
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


#include "spd/SPDPolyGroundFilter.h"


namespace spdlib
{
    

    SPDPolyFitGroundFilter::SPDPolyFitGroundFilter()
    {
        
    }
    
    void SPDPolyFitGroundFilter::applyPolyFitGroundFilter(string inputFile, string outputFile, float grdThres, boost::uint_fast16_t degree, boost::uint_fast16_t iters, boost::uint_fast32_t blockXSize, boost::uint_fast32_t blockYSize, float processingResolution)throw(SPDProcessingException)
    {
        try 
        {
            // Read SPD file header.
            SPDFile *spdFile = new SPDFile(inputFile);
            SPDFileReader spdReader;
            spdReader.readHeaderInfo(spdFile->getFilePath(), spdFile);
                        
            // Find the minimum returns.
            vector<SPDPoint*> *minPts = new vector<SPDPoint*>();
            
            SPDPulseProcessor *processorMinPoints = new SPDFindMinReturnsProcessor(minPts);            
            SPDSetupProcessPulses processPulses = SPDSetupProcessPulses(blockXSize, blockYSize, true);
            processPulses.processPulses(processorMinPoints, spdFile, processingResolution);
            delete processorMinPoints;
            
            // Create grid to store returns.
            vector<SPDPoint*> ***minPtGrid = new vector<SPDPoint*>**[spdFile->getNumberBinsY()];
            for(boost::uint_fast32_t i = 0; i < spdFile->getNumberBinsY(); ++i)
            {
                minPtGrid[i] = new vector<SPDPoint*>*[spdFile->getNumberBinsX()];
                for(boost::uint_fast32_t j = 0; j < spdFile->getNumberBinsX(); ++j)
                {
                    minPtGrid[i][j] = new vector<SPDPoint*>();
                }
            }
            
            // Grid returns.
            this->buildMinGrid(spdFile, minPts, minPtGrid);
            
            /********** Find Surface ***************/
			
			// calculate number of coefficients
			//unsigned int degree=2; // this will be user defined via command prompt in final version
			//this->degree = degree;
			//this->iters = iters;
			unsigned int Ncoeffs;
			//int l=0;
			//int p=0;
			int ItersCount=0;
			
			
			if (degree > 1) 
			{
                Ncoeffs=((degree+1)*(2*degree))/2;
			}
			else {
				Ncoeffs=3; // plane fit (Const+Ax+By) hardcoded because above equation only works for orders of 2 or more
			}
            
			// Set up matrix of powers
			gsl_matrix *indVarPow;
			gsl_vector *depVar;
			gsl_vector *outCoefficients;
			gsl_vector *SurfaceZ;
            
			
			indVarPow = gsl_matrix_alloc(spdFile->getNumberBinsX()*spdFile->getNumberBinsY(),Ncoeffs); // Matrix to hold powers of x,y
			depVar = gsl_vector_alloc(spdFile->getNumberBinsX()*spdFile->getNumberBinsY()); // Vector to hold z term
			outCoefficients = gsl_vector_alloc(Ncoeffs); // Vector to hold output coefficients
            SurfaceZ = gsl_vector_alloc(spdFile->getNumberBinsX()*spdFile->getNumberBinsY());
			
			gsl_multifit_linear_workspace *workspace;
			workspace = gsl_multifit_linear_alloc(spdFile->getNumberBinsX()*spdFile->getNumberBinsY(), Ncoeffs);
			gsl_matrix *cov;
			double chisq;
			cov = gsl_matrix_alloc(Ncoeffs, Ncoeffs);
			
			//gsl_permutation * perm = gsl_permutation_alloc (spdFile->getNumberBinsX()*spdFile->getNumberBinsY());
            
            
			bool keepProcessing = true;
			
			cout << "Number of Y bins: " << spdFile->getNumberBinsY() << '\n';
			cout << "Number of X bins: " << spdFile->getNumberBinsX() << '\n';
			
			while(keepProcessing)
			{
				boost::uint_fast32_t p=0;
				for(boost::uint_fast32_t i = 0; i < spdFile->getNumberBinsY(); ++i)
				{
					for(boost::uint_fast32_t j = 0; j < spdFile->getNumberBinsX(); ++j)
					{
						//cout << "i,j : " << i << " " << j << '\n';
						//cout << "Number of Y bins: " << spdFile->getNumberBinsY() << '\n';
						//cout << "Number of X bins: " << spdFile->getNumberBinsX() << '\n';
                        
						
						if(minPtGrid[i][j]->size() > 0)
						{
							//cout << "Pt: " << minPtGrid[i][j]->front()->x << ", " << minPtGrid[i][j]->front()->y << ", " << minPtGrid[i][j]->front()->z << '\n';
							// generate arrays for x^ny^m and z
							double xelement=minPtGrid[i][j]->front()->x;
							double yelement=minPtGrid[i][j]->front()->y;
							double zelement=minPtGrid[i][j]->front()->z;
							boost::uint_fast32_t l=0;
							gsl_vector_set(depVar, p, zelement);
							//++p;
							
							//cout << "NCoeffs: " << Ncoeffs << " Degree: " << degree << '\n';
							
							for (boost::uint_fast32_t m = 0; m < Ncoeffs ; m++)
							{
								for (boost::uint_fast32_t n=0; n < Ncoeffs ; n++)
								{
									if (n+m <= degree)
									{
										double xelementtPow = pow(xelement, ((int)(m)));
										double yelementtPow = pow(yelement, ((int)(n)));
										gsl_matrix_set(indVarPow,p,l, xelementtPow*yelementtPow); // was n,m instead of l
                                                                                                  //cout << "indvarpow: " << indVarPow << " " << xelementtPow << " " << yelementtPow << " " << "n,m: " << n << " " << m << '\n';
                                                                                                  //cout << "n,m: " << n << " " << m << " " << l << '\n';
										++l;
									}
								}
							}
							++p;
							
						}
					}
					
				}
				
				
				//cout << "Iters count" << ItersCount << " " << keepProcessing << '\n';
				
				
				// Find surface
				//matrix operations to find coefficients
				// K=U.inverse(Utrans.U)
				//Coeffs=K.z  
				
				//LU = gsl_linalg_LU_decomp (gsl_matrix * indVarPow, gsl_permutation * perm, int * signum);
				//gsl_linalg_LU_solve (const gsl_matrix * LU, const gsl_permutation * perm, const gsl_vector * depVar, gsl_vector * outCoefficients);
				
				// Perform Least Squared Fit
				
                
				gsl_multifit_linear(indVarPow, depVar, outCoefficients, cov, &chisq, workspace);
				
				// calculate vector of z values from outCoefficients and indVarPow to compare with original data in next bit...
				//compute SurfaceZ = indVarPow.outCoefficients
				//gsl_blas_dgemv (CblasNoTrans, CblasNoTrans, 1.0, indVarPow, outCoefficients, 0.0, SurfaceZ);
				
				gsl_blas_dgemv (CblasNoTrans, 1.0, indVarPow, outCoefficients, 0.0, SurfaceZ);
				
				// compare polynomial surface with minimum points grid.
				// if there are points above a certain threshold, delete them
				// and then start the interation again to find a better surface fit for the ground.
				// this step is necessary to remove minimum points in branches residing over shadowed areas.
                
				
                for(unsigned int j = 0; j < outCoefficients->size; j++)
                {
                    double outm = gsl_vector_get(outCoefficients, j); 
                    cout << outm << " , " ;
                }
                cout << '\n';
				
				
				
				
				// remove outlying points that are obviously not ground
				
				for(boost::uint_fast32_t i = 0; i < spdFile->getNumberBinsY(); ++i)
				{
					for(boost::uint_fast32_t j = 0; j < spdFile->getNumberBinsX(); ++j)
					{
						if(minPtGrid[i][j]->size() > 0)
						{
							
							// to determine if points are above surface:
							double zelement=minPtGrid[i][j]->front()->z;
							double xelement=minPtGrid[i][j]->front()->x;
							double yelement=minPtGrid[i][j]->front()->y;
							double sz=0; // reset z value from surface coefficients
							boost::uint_fast32_t l=0;
							
							for (boost::uint_fast32_t m = 0; m < Ncoeffs ; m++)
							{
								for (boost::uint_fast32_t n=0; n < Ncoeffs ; n++)
								{
									if (n+m <= degree)
									{
										double xelementtPow = pow(xelement, ((int)(m)));
										double yelementtPow = pow(yelement, ((int)(n)));
										double outm = gsl_vector_get(outCoefficients, l);
										
										sz=sz+(outm*xelementtPow*yelementtPow);
										++l;
									}
								}
							}
							
							
							if (zelement-sz > 1.0)
							{
								minPtGrid[i][j]->clear(); // delete point
							}
							
							++l;
                            
                            
						}
					}
					
				}
				
				ItersCount++;
				if (ItersCount==iters)
				{
					keepProcessing = false;
				}
				
				
			}  
            // end of while loop
			
			
            // Clean up	
            gsl_multifit_linear_free(workspace);
			gsl_matrix_free(cov);
            gsl_matrix_free(indVarPow);
            gsl_vector_free(depVar);
            /***************************************/
            
            
            // Remove minimum returns grid and minimum returns from memory
            for(boost::uint_fast32_t i = 0; i < spdFile->getNumberBinsY(); ++i)
            {
                for(boost::uint_fast32_t j = 0; j < spdFile->getNumberBinsX(); ++j)
                {
                    delete minPtGrid[i][j];
                }
                delete[] minPtGrid[i];
            }
            delete[] minPtGrid;
            
            for(vector<SPDPoint*>::iterator iterPts = minPts->begin(); iterPts != minPts->end(); )
            {
                delete *iterPts;
                iterPts = minPts->erase(iterPts);
            }
            delete minPts;
            
            // Classify ground returns using identified surface.
            SPDPulseProcessor *processorClassFromSurface = new SPDClassifyGrdReturnsFromSurfaceCoefficientsProcessor(grdThres, degree, iters, outCoefficients);            
            processPulses.processPulsesWithOutputSPD(processorClassFromSurface, spdFile, outputFile, processingResolution);
            delete processorClassFromSurface;
        } 
        catch (SPDProcessingException &e) 
        {
            throw e;
        }
        catch(SPDException &e)
        {
            throw SPDProcessingException(e.what());
        }

    }
    
    void SPDPolyFitGroundFilter::buildMinGrid(SPDFile *spdFile, vector<SPDPoint*> *minPts, vector<SPDPoint*> ***minPtGrid)throw(SPDProcessingException)
    {
        if(minPts->size() > 0)
		{
            double binSize = spdFile->getBinSize();
            boost::uint_fast32_t xBins = spdFile->getNumberBinsX();
            boost::uint_fast32_t yBins = spdFile->getNumberBinsY();
            
            if((xBins < 1) | (yBins < 1))
			{
				throw SPDProcessingException("There insufficent number of bins for binning (try reducing resolution).");
			}
            
            double tlX = spdFile->getXMin();
            double tlY = spdFile->getYMax();
            
			vector<SPDPoint*>::iterator iterPts;
			
			try 
			{	
				double xDiff = 0;
				double yDiff = 0;
				boost::uint_fast32_t xIdx = 0;
				boost::uint_fast32_t yIdx = 0;
				
				SPDPoint *pt = NULL;
                vector<SPDPoint*>::iterator iterPts;
				for(iterPts = minPts->begin(); iterPts != minPts->end(); ++iterPts)
				{
					pt = *iterPts;
					
					xDiff = (pt->x - tlX)/binSize;
					yDiff = (tlY - pt->y)/binSize;				
					
					try 
					{
						xIdx = numeric_cast<boost::uint_fast32_t>(xDiff);
						yIdx = numeric_cast<boost::uint_fast32_t>(yDiff);
					}
					catch(negative_overflow& e) 
					{
						throw SPDProcessingException(e.what());
					}
					catch(positive_overflow& e) 
					{
						throw SPDProcessingException(e.what());
					}
					catch(bad_numeric_cast& e) 
					{
						throw SPDProcessingException(e.what());
					}
					
					if(xIdx > (xBins-1))
					{
                        --xIdx;
                        if(xIdx > (xBins-1))
                        {
                            cout << "Point: [" << pt->x << "," << pt->y << "]\n";
                            cout << "Diff [" << xDiff << "," << yDiff << "]\n";
                            cout << "Index [" << xIdx << "," << yIdx << "]\n";
                            cout << "Size [" << xBins << "," << yBins << "]\n";
                            throw SPDProcessingException("Did not find x index within range.");
                        }
					}
					
					if(yIdx > (yBins-1))
					{
                        --yIdx;
                        if(yIdx > (yBins-1))
                        {
                            cout << "Point: [" << pt->x << "," << pt->y << "]\n";
                            cout << "Diff [" << xDiff << "," << yDiff << "]\n";
                            cout << "Index [" << xIdx << "," << yIdx << "]\n";
                            cout << "Size [" << xBins << "," << yBins << "]\n";
                            throw SPDProcessingException("Did not find y index within range.");
                        }
					}
					
					minPtGrid[yIdx][xIdx]->push_back(pt);
				}
			}
			catch (SPDProcessingException &e) 
			{
				throw e;
			}
			
		}
		else 
		{
			throw SPDProcessingException("Inputted list of points was empty.");
		}
    }
    
    SPDPolyFitGroundFilter::~SPDPolyFitGroundFilter()
    {
        
    }
    
}



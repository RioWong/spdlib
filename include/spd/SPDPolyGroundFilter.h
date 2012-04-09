/*
 *  SPDPolyGroundFilter.h
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

#ifndef SPDPolyGroundFilter_H
#define SPDPolyGroundFilter_H

#include <iostream>
#include <string>
#include <list>
#include "math.h"

#include "spd/SPDFile.h"
#include "spd/SPDPoint.h"
#include "spd/SPDPulse.h"
#include "spd/SPDProcessPulses.h"
#include "spd/SPDPulseProcessor.h"
#include "spd/SPDProcessingException.h"


#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_blas.h>

using namespace std;

namespace spdlib
{
	
	class SPDFindMinReturnsProcessor : public SPDPulseProcessor
	{
	public:
        SPDFindMinReturnsProcessor(vector<SPDPoint*> *minPts)
        {
            this->minPts = minPts;
        };
        
        void processDataColumnImage(SPDFile *inSPDFile, vector<SPDPulse*> *pulses, float *imageData, SPDXYPoint *cenPts, boost::uint_fast32_t numImgBands, float binSize) throw(SPDProcessingException)
		{throw SPDProcessingException("Processing with an output image is not implemented.");};
        
        void processDataColumn(SPDFile *inSPDFile, vector<SPDPulse*> *pulses, SPDXYPoint *cenPts) throw(SPDProcessingException)
        {
            SPDPoint *minPt = NULL;
            float minZ = 0;
            bool first = true;
            
            for(vector<SPDPulse*>::iterator iterPulses = pulses->begin(); iterPulses != pulses->end(); ++iterPulses)
            {
                if((*iterPulses)->numberOfReturns > 0)
                {
                    for(vector<SPDPoint*>::iterator iterPoints = (*iterPulses)->pts->begin(); iterPoints != (*iterPulses)->pts->end(); ++iterPoints)
                    {
                        if(first)
                        {
                            minPt = *iterPoints;
                            minZ = (*iterPoints)->z;
                            first = false;
                        }
                        else if((*iterPoints)->z < minZ)
                        {
                            minPt = *iterPoints;
                            minZ = (*iterPoints)->z;
                        }
                    }
                }
            }
            
            if(!first)
            {
                SPDPoint *pt = new SPDPoint();
                SPDPointUtils ptUtils;
                ptUtils.copySPDPointTo(minPt, pt);
                minPts->push_back(pt);
            }
        };
        
        void processDataWindowImage(SPDFile *inSPDFile, vector<SPDPulse*> ***pulses, float ***imageData, SPDXYPoint ***cenPts, boost::uint_fast32_t numImgBands, boost::uint_fast16_t winSize) throw(SPDProcessingException)
        {throw SPDProcessingException("Processing using a window is not implemented.");};
        
		void processDataWindow(SPDFile *inSPDFile, vector<SPDPulse*> ***pulses, SPDXYPoint ***cenPts, boost::uint_fast16_t winSize) throw(SPDProcessingException)
        {throw SPDProcessingException("Processing using a window is not implemented.");};
        
        vector<string> getImageBandDescriptions() throw(SPDProcessingException)
        {
            return vector<string>();
        };
        
        void setHeaderValues(SPDFile *spdFile) throw(SPDProcessingException)
        {};
        
        ~SPDFindMinReturnsProcessor(){};
    protected:
        vector<SPDPoint*> *minPts;
	};

    
    class SPDClassifyGrdReturnsFromSurfaceCoefficientsProcessor : public SPDPulseProcessor
	{
	public:
        SPDClassifyGrdReturnsFromSurfaceCoefficientsProcessor(float grdThres, boost::uint_fast16_t degree, boost::uint_fast16_t iters, gsl_vector *coefficients)
        {
            this->grdThres = grdThres;
            this->degree = degree;
            this->iters = iters;
            this->coefficients = coefficients;
        };
        
        void processDataColumnImage(SPDFile *inSPDFile, vector<SPDPulse*> *pulses, float *imageData, SPDXYPoint *cenPts, boost::uint_fast32_t numImgBands, float binSize) throw(SPDProcessingException)
		{throw SPDProcessingException("Processing with an output image is not implemented.");};
        
        void processDataColumn(SPDFile *inSPDFile, vector<SPDPulse*> *pulses, SPDXYPoint *cenPts) throw(SPDProcessingException)
        {
            for(vector<SPDPulse*>::iterator iterPulses = pulses->begin(); iterPulses != pulses->end(); ++iterPulses)
            {
                if((*iterPulses)->numberOfReturns > 0)
                {
                    for(vector<SPDPoint*>::iterator iterPoints = (*iterPulses)->pts->begin(); iterPoints != (*iterPulses)->pts->end(); ++iterPoints)
                    {
                        // Remove any existing ground return classification.
                        if((*iterPoints)->classification == SPD_GROUND)
                        {
                            (*iterPoints)->classification = SPD_UNCLASSIFIED;
                        }
                        
                        // Calc surface height for return
                        double xcoord= (*iterPoints)->x;
                        double ycoord= (*iterPoints)->y;
                        double zcoord= (*iterPoints)->z;
                        double surfaceValue=0; // reset z value from surface coefficients
                        boost::uint_fast32_t l=0;
                        
                        for (boost::uint_fast32_t m = 0; m < coefficients->size ; m++)
                        {
                            for (boost::uint_fast32_t n=0; n < coefficients->size ; n++)
                            {
                                if (n+m <= degree)
                                {
                                    double xelementtPow = pow(xcoord, ((int)(m)));
                                    double yelementtPow = pow(ycoord, ((int)(n)));
                                    double outm = gsl_vector_get(coefficients, l);
                                    
                                    surfaceValue=surfaceValue+(outm*xelementtPow*yelementtPow);
                                    ++l;
                                }
                            }
                        }
                        
                        // Is return height less than surface height + grdThres
                        // sqrt((zcoord-surfaceValue)*(zcoord-surfaceValue)) <= grdThres
                        
                        if ((zcoord-surfaceValue) <= grdThres) {
                            (*iterPoints)->classification = SPD_GROUND;
                        }
                        
                        
                    }
                }
            }
        };
        
        void processDataWindowImage(SPDFile *inSPDFile, vector<SPDPulse*> ***pulses, float ***imageData, SPDXYPoint ***cenPts, boost::uint_fast32_t numImgBands, boost::uint_fast16_t winSize) throw(SPDProcessingException)
        {throw SPDProcessingException("Processing using a window is not implemented.");};
        
		void processDataWindow(SPDFile *inSPDFile, vector<SPDPulse*> ***pulses, SPDXYPoint ***cenPts, boost::uint_fast16_t winSize) throw(SPDProcessingException)
        {throw SPDProcessingException("Processing using a window is not implemented.");};
        
        vector<string> getImageBandDescriptions() throw(SPDProcessingException)
        {
            return vector<string>();
        };
        
        void setHeaderValues(SPDFile *spdFile) throw(SPDProcessingException)
        {};
        
        ~SPDClassifyGrdReturnsFromSurfaceCoefficientsProcessor(){};
    protected:
        float grdThres;
        boost::uint_fast16_t degree;
        boost::uint_fast16_t iters;
        gsl_vector *coefficients;
	};

    
    class SPDPolyFitGroundFilter
	{
	public:
		SPDPolyFitGroundFilter();
        void applyPolyFitGroundFilter(string inputFile, string outputFile, float grdThres,boost::uint_fast16_t degree,boost::uint_fast16_t iters, boost::uint_fast32_t blockXSize=250, boost::uint_fast32_t blockYSize=250, float processingResolution=0)throw(SPDProcessingException);
		~SPDPolyFitGroundFilter();
    private:
        void buildMinGrid(SPDFile *spdFile, vector<SPDPoint*> *minPts, vector<SPDPoint*> ***minPtGrid)throw(SPDProcessingException);
	};
    
    
}


#endif


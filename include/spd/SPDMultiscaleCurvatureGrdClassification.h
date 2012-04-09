/*
 *  SPDMultiscaleCurvatureGrdClassification.h
 *
 *  Created by Pete Bunting on 03/03/2012.
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

#ifndef SPDMultiscaleCurvatureGrdClassification_H
#define SPDMultiscaleCurvatureGrdClassification_H

#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include "math.h"

#include <boost/cstdint.hpp>

#include "spd/SPDCommon.h"
#include "spd/SPDFile.h"
#include "spd/SPDPoint.h"
#include "spd/SPDPulse.h"
#include "spd/SPDProcessingException.h"
#include "spd/SPDDataBlockProcessor.h"
#include "spd/SPDPointGridIndex.h"

#include "spd/tps/spline.h"
#include "spd/tps/linalg3d.h"
#include "spd/tps/ludecomposition.h"

#include "boost/math/special_functions/fpclassify.hpp"

using namespace std;
using namespace spdlib::tps;

namespace spdlib
{
    enum SPDSmoothFilterType
    {
        meanFilter = 0,
        medianFilter = 1
    };
    
    class SPDMultiscaleCurvatureGrdClassification : public SPDDataBlockProcessor
	{
	public:
        SPDMultiscaleCurvatureGrdClassification(float initScale,boost::uint_fast16_t numOfScalesAbove,boost::uint_fast16_t numOfScalesBelow, float scaleGaps, float initCurveTolerance, float minCurveTolerance, float stepCurveTolerance, float interpMaxRadius,boost::uint_fast16_t interpNumPoints, SPDSmoothFilterType filterType,boost::uint_fast16_t smoothFilterHSize, float thresOfChange, bool multiReturnPulsesOnly,boost::uint_fast16_t classParameters);
        void processDataBlockImage(SPDFile *inSPDFile, vector<SPDPulse*> ***pulses, float ***imageDataBlock, SPDXYPoint ***cenPts, boost::uint_fast32_t xSize, boost::uint_fast32_t ySize, boost::uint_fast32_t numImgBands, float binSize) throw(SPDProcessingException)
		{throw SPDProcessingException("SPDMultiscaleCurvatureGrdClassification cannot output an image layer.");};
        
        void processDataBlock(SPDFile *inSPDFile, vector<SPDPulse*> ***pulses, SPDXYPoint ***cenPts, boost::uint_fast32_t xSize, boost::uint_fast32_t ySize) throw(SPDProcessingException);
        
        void processDataBlockImage(SPDFile *inSPDFile, vector<SPDPulse*> *pulses, float ***imageDataBlock, SPDXYPoint ***cenPts, boost::uint_fast32_t xSize, boost::uint_fast32_t ySize, boost::uint_fast32_t numImgBands) throw(SPDProcessingException)
		{throw SPDProcessingException("SPDMultiscaleCurvatureGrdClassification requires processing with a grid.");};
        
        void processDataBlock(SPDFile *inSPDFile, vector<SPDPulse*> *pulses) throw(SPDProcessingException)
        {throw SPDProcessingException("SPDMultiscaleCurvatureGrdClassification requires processing with a grid.");};
        
        vector<string> getImageBandDescriptions() throw(SPDProcessingException)
        {
            vector<string> bandNames;
            bandNames.push_back("MCC Surface");
            return bandNames;
        }
        void setHeaderValues(SPDFile *spdFile) throw(SPDProcessingException)
        {
            // Nothing to do...
        }

        ~SPDMultiscaleCurvatureGrdClassification();
        
    protected:
        double* findDataExtentAndClassifyAllPtsAsGrd(vector<SPDPulse*> ***pulses,boost::uint_fast32_t xSizePulses,boost::uint_fast32_t ySizePulses) throw(SPDProcessingException);
        float** createElevationRaster(double *bbox, float rasterScale,boost::uint_fast32_t *xSizeRaster,boost::uint_fast32_t *ySizeRaster, vector<SPDPulse*> ***pulses,boost::uint_fast32_t xSizePulses,boost::uint_fast32_t ySizePulses) throw(SPDProcessingException);
        void smoothMeanRaster(float **raster,boost::uint_fast32_t xSizeRaster,boost::uint_fast32_t ySizeRasterr,boost::uint_fast16_t filterHSize) throw(SPDProcessingException);
        void smoothMedianRaster(float **raster,boost::uint_fast32_t xSizeRaster,boost::uint_fast32_t ySizeRasterr,boost::uint_fast16_t filterHSize) throw(SPDProcessingException);
        float classifyNonGrdPoints(float curveTolerance, double *bbox, float rasterScale, float **raster,boost::uint_fast32_t xSizeRaster,boost::uint_fast32_t ySizeRaster, vector<SPDPulse*> ***pulses,boost::uint_fast32_t xSizePulses,boost::uint_fast32_t ySizePulses) throw(SPDProcessingException);
        float initScale;
        boost::uint_fast16_t numOfScalesAbove;
        boost::uint_fast16_t numOfScalesBelow;
        float scaleGaps;
        float initCurveTolerance;
        float minCurveTolerance;
        float stepCurveTolerance;
        float interpMaxRadius;
        boost::uint_fast16_t interpNumPoints;
        SPDSmoothFilterType filterType;
        boost::uint_fast16_t smoothFilterHSize;
        float thresOfChange;
        bool multiReturnPulsesOnly;
        boost::uint_fast16_t classParameters;
	};
    
    
    
    class SPDTPSNumPtsUseAvThinInterpolator
	{
	public:
		SPDTPSNumPtsUseAvThinInterpolator(float radius,boost::uint_fast16_t numPoints,boost::uint_fast16_t elevVal, double gridResolution, bool thinGrid);
		void initInterpolator(list<SPDPulse*> ***pulses,boost::uint_fast32_t numXBins,boost::uint_fast32_t numYBins,boost::uint_fast16_t ptClass) throw(SPDProcessingException);
		void initInterpolator(vector<SPDPulse*> ***pulses,boost::uint_fast32_t numXBins,boost::uint_fast32_t numYBins,boost::uint_fast16_t ptClass) throw(SPDProcessingException);
		void initInterpolator(list<SPDPulse*> *pulses,boost::uint_fast16_t ptClass) throw(SPDProcessingException);
		void initInterpolator(vector<SPDPulse*> *pulses,boost::uint_fast16_t ptClass) throw(SPDProcessingException);
		float getValue(double eastings, double northings) throw(SPDProcessingException);
		void resetInterpolator() throw(SPDProcessingException);
		~SPDTPSNumPtsUseAvThinInterpolator();
	protected:
        bool initialised;
		SPDPointGridIndex *idx;
		vector<SPDPoint*> *pts;
		double gridResolution;
        bool thinGrid;
        float radius;
        boost::uint_fast16_t numPoints;
        boost::uint_fast16_t elevVal;
	};
    
    
}



#endif



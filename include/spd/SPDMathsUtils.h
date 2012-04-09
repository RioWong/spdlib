/*
 *  SPDMathsUtils.h
 *  SPDLIB
 *
 *  Created by Pete Bunting on 22/06/2011.
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


#ifndef SPDMathsUtils_H
#define SPDMathsUtils_H

#include <iostream>
#include <math.h>
#include <vector>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_linalg.h>

#include "spd/cmpfit/mpfit.h"

#include "spd/SPDCommon.h"
#include "spd/SPDException.h"
#include "spd/SPDProcessingException.h"

using namespace std;

namespace spdlib
{
    
    struct GaussianDecompReturnType
    {
        float gaussianAmplitude;
        float gaussianWidth;
        float axisInterval;
    };
    
    class SPDInitDecomposition
	{
	public:
		SPDInitDecomposition(){};
		virtual vector<uint_fast32_t>* findInitPoints(uint_fast32_t *waveform,boost::uint_fast16_t waveformLength, float intThreshold) throw(SPDException) = 0;
        virtual vector<uint_fast32_t>* findInitPoints(float *waveform,boost::uint_fast16_t waveformLength, float intThreshold) throw(SPDException) = 0;
		virtual ~SPDInitDecomposition(){};
	};
	
	class SPDInitDecompositionZeroCrossingSimple : public SPDInitDecomposition
	{
	public:
		SPDInitDecompositionZeroCrossingSimple(float decay);
		vector<uint_fast32_t>* findInitPoints(uint_fast32_t *waveform,boost::uint_fast16_t waveformLength, float intThreshold) throw(SPDException);
        vector<uint_fast32_t>* findInitPoints(float *waveform,boost::uint_fast16_t waveformLength, float intThreshold) throw(SPDException);
		~SPDInitDecompositionZeroCrossingSimple();
	private:
		float decay;
	};
	
	class SPDInitDecompositionZeroCrossing : public SPDInitDecomposition
	{
	public:
		SPDInitDecompositionZeroCrossing(float decay,boost::uint_fast32_t intDecayThres);
		vector<uint_fast32_t>* findInitPoints(uint_fast32_t *waveform,boost::uint_fast16_t waveformLength, float intThreshold) throw(SPDException);
        vector<uint_fast32_t>* findInitPoints(float *waveform,boost::uint_fast16_t waveformLength, float intThreshold) throw(SPDException);
		~SPDInitDecompositionZeroCrossing();
	private:
		bool zeroCrossing(float grad1, float grad2);
		float decay;
	boost::uint_fast32_t intDecayThres;
	};
    
    class SPDInitDecompositionZeroCrossingNoRinging : public SPDInitDecomposition
	{
	public:
		SPDInitDecompositionZeroCrossingNoRinging();
		vector<uint_fast32_t>* findInitPoints(uint_fast32_t *waveform,boost::uint_fast16_t waveformLength, float intThreshold) throw(SPDException);
        vector<uint_fast32_t>* findInitPoints(float *waveform,boost::uint_fast16_t waveformLength, float intThreshold) throw(SPDException);
		~SPDInitDecompositionZeroCrossingNoRinging();
	private:
		bool zeroCrossing(float grad1, float grad2);
	};
    
    class SPDMathsUtils
    {
    public: 
        SPDMathsUtils();
        /// dataValuesY is the list to be smoothed while dataValuesX are the corresponding
        /// values for the other axis (e.g., height).
        void applySavitzkyGolaySmoothing(float *dataValuesY, float *dataValuesX,boost::uint_fast32_t numValues,boost::uint_fast16_t winHSize,boost::uint_fast16_t order, bool removeLTZeros) throw(SPDProcessingException);
        /// fitGaussianMixture is a function which fits are number of Gaussians (the number and starting points are defined
        /// by the SPDInitDecomposition class passed to the function) to the dataValues.
        vector<GaussianDecompReturnType*>* fitGaussianMixture(SPDInitDecomposition *initDecomp, float minimumGaussianGap, float *dataValues, float *dataIntervals,boost::uint_fast32_t nVals, float intThreshold) throw(SPDProcessingException);
        /// decomposeSingleGaussian is a function which fits a Gaussian to the inputted data values using 
        /// the maximum peak as the starting point.
        void decomposeSingleGaussian(uint_fast32_t *waveform,boost::uint_fast16_t waveformLength,boost::uint_fast16_t waveFitWindow, float waveformTimeInterval, float *transAmp, float *transWidth, float *peakTime) throw(SPDProcessingException);
        ~SPDMathsUtils();
    };
    
    /// This class has been copied from RSGISLib (RSGISSingularValueDecomposition) 
    /// and was originally created by Daniel Clewely. 
    class SPDSingularValueDecomposition
    {
    public:
        SPDSingularValueDecomposition();
        void ComputeSVDgsl(gsl_matrix *inA);
        void SVDLinSolve(gsl_vector *outX, gsl_vector *inB);
        ~SPDSingularValueDecomposition();
    private:
        int numcoefficients;
        int svdCompute;
        int svdSolve;
        gsl_vector *outX;
        gsl_matrix *inA;
        gsl_matrix *outV;
        gsl_vector *outS;
    };
    
    
	/// Class to perform polynomaial fitting
    /// This class has been copied from RSGISLib (RSGISPolyFit) 
    /// and was originally created by Daniel Clewely. 
	class SPDPolyFit
    {
    public:
        SPDPolyFit();
        gsl_vector* PolyfitOneDimensionQuiet(int order, gsl_matrix *inData);
        gsl_vector* PolyfitOneDimension(int order, gsl_matrix *inData);
        gsl_vector* PolyfitOneDimensionSVD(int order, gsl_matrix *inData);
        gsl_matrix* PolyTestOneDimension(int order, gsl_matrix *inData, gsl_vector *coefficients);
        gsl_matrix* PolyfitTwoDimension(int numX, int numY, int orderX, int orderY, gsl_matrix *inData);
        gsl_matrix* PolyTestTwoDimension(int orderX, int orderY, gsl_matrix *inData, gsl_matrix *coefficeints);
        gsl_matrix* PolyfitThreeDimension(int numX, int numY, int numZ, int orderX, int orderY, int orderZ, gsl_matrix *inData);
        gsl_matrix* PolyTestThreeDimension(int orderX, int orderY, int orderZ, gsl_matrix *inData, gsl_matrix *coefficients);
        void calcRSquaredGSLMatrix(gsl_matrix *dataXY);
        void calcRMSErrorGSLMatrix(gsl_matrix *dataXY);
        void calcMeanErrorGSLMatrix(gsl_matrix *dataXY);
        double calcRSquaredGSLMatrixQuiet(gsl_matrix *dataXY);
        double calcRMSErrorGSLMatrixQuiet(gsl_matrix *dataXY);
        double calcMeanErrorGSLMatrixQuiet(gsl_matrix *dataXY);
        ~SPDPolyFit();
    private:
        int order;
        gsl_matrix inData;
        int numX;
        int numY;
    };
}

#endif



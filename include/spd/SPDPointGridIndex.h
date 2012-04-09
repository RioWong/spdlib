/*
 *  SPDPointGridIndex.h
 *  SPDLIB
 *
 *  Created by Pete Bunting on 08/03/2011.
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
 */

#ifndef SPDPointGridIndex_H
#define SPDPointGridIndex_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#include <boost/cstdint.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

#include "spd/SPDFile.h"
#include "spd/SPDPoint.h"
#include "spd/SPDProcessingException.h"

using namespace std;
using boost::numeric_cast;
using boost::numeric::bad_numeric_cast;
using boost::numeric::positive_overflow;
using boost::numeric::negative_overflow;

namespace spdlib
{  
    static double currentCmpEastings = 0;
    static double currentCmpNorthings = 0;
    
    inline bool compareFuncSortByDistanceTo(SPDPoint *pt1, SPDPoint *pt2)
    {
        SPDPointUtils ptUtils;
        double dist2Pt1 = ptUtils.distanceXY(currentCmpEastings, currentCmpNorthings, pt1);
        double dist2Pt2 = ptUtils.distanceXY(currentCmpEastings, currentCmpNorthings, pt2);
        
        return dist2Pt1 > dist2Pt2;
    };
    
    inline bool compareFuncSortByZLargestFirst(SPDPoint *pt1, SPDPoint *pt2)
    {       
        return pt1->z > pt2->z;
    };
    
    inline bool compareFuncSortByZSmallestFirst(SPDPoint *pt1, SPDPoint *pt2)
    {       
        return pt1->z < pt2->z;
    };
    
    inline bool compareFuncSortByHeightLargestFirst(SPDPoint *pt1, SPDPoint *pt2)
    {       
        return pt1->height > pt2->height;
    };
    
    inline bool compareFuncSortByHeightSmallestFirst(SPDPoint *pt1, SPDPoint *pt2)
    {       
        return pt1->height < pt2->height;
    };
    
	class SPDPointGridIndex
	{
	public:
		SPDPointGridIndex();
		void buildIndex(vector<SPDPoint*> *pts, double binSize, OGREnvelope *env) throw(SPDProcessingException);
        void buildIndex(vector<SPDPoint*> *pts, double binSize) throw(SPDProcessingException);
		bool getPointsInRadius(vector<SPDPoint*> *pts, double eastings, double northings, double radius) throw(SPDProcessingException);
        bool getSetNumOfPoints(vector<SPDPoint*> *pts, double eastings, double northings,boost::uint_fast16_t numPts, double maxRadius) throw(SPDProcessingException);
        void thinPtsInBins(uint_fast16_t elevVal,boost::uint_fast16_t selectHighOrLow,boost::uint_fast16_t maxNumPtsPerBin) throw(SPDProcessingException);
        void thinPtsInBinsWithDelete(uint_fast16_t elevVal,boost::uint_fast16_t selectHighOrLow,boost::uint_fast16_t maxNumPtsPerBin) throw(SPDProcessingException);
        void thinPtsWithAvZ(uint_fast16_t elevVal) throw(SPDProcessingException);
        void getAllPointsInGrid(vector<SPDPoint*> *pts) throw(SPDProcessingException);
       boost::uint_fast32_t getXBins(){return xBins;};
       boost::uint_fast32_t getYBins(){return yBins;};
		~SPDPointGridIndex();
	private:
		vector<SPDPoint*> ***ptGrid;
		double tlX;
		double tlY;
		double brX;
		double brY;
		double binSize;
	boost::uint_fast32_t xBins;
	boost::uint_fast32_t yBins;
        bool deletePtsInBins;
	};
}

#endif





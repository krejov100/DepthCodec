//
// Created by philip on 8/9/18.
//

#ifndef DEPTHCODEC_QUADTREETYPES_H
#define DEPTHCODEC_QUADTREETYPES_H

#include <memory>
#include <opencv2/core/mat.hpp>
#include "NodeAddress.h"
#include "RollingQuadTree.h"
#include "TiledCodec.h"
#include "Range.h"

class AbsDiffPolicy{
    short mThresh;
protected:
    template<typename LEAF_DATA_TYPE>
    bool shouldPrune(LEAF_DATA_TYPE leafData){
        if(leafData.getMin() == 0)
            return true;
        return abs(leafData.getMin()-leafData.getMax()) <= mThresh ;
    }

    AbsDiffPolicy(){};
public:
    AbsDiffPolicy(short thresh):mThresh(thresh){}
	
	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
    }

	friend boost::serialization::access;
};

using RollingQT32bitMinMaxAbsDiff = RollingQuadTree<NodeAddress32bit, Range<unsigned short>, AbsDiffPolicy>;
using RollingQT16bitMinMaxAbsDiff = RollingQuadTree<NodeAddress16bit, Range<unsigned short>, AbsDiffPolicy>;





#endif //DEPTHCODEC_QUADTREETYPES_H

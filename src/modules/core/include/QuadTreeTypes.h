//
// Created by philip on 8/9/18.
//

#ifndef DEPTHCODEC_QUADTREETYPES_H
#define DEPTHCODEC_QUADTREETYPES_H

#include <memory>
#include <opencv2/core/mat.hpp>
#include "NodeAddress.h"
#include "RollingQuadTree.h"
#include "MultipleQuadTree.h"

class AbsDiffPolicy{
    short mThresh;
protected:
    template<typename LEAF_DATA_TYPE>
    bool shouldPrune(LEAF_DATA_TYPE leafData){
        //if(leafData.min == 0)
        //    return true;
        return abs(leafData.min-leafData.max) <= mThresh ;
    }

    AbsDiffPolicy(){};
public:
    AbsDiffPolicy(short thresh):mThresh(thresh){}
};


class MinMax{
public:
    unsigned short min;
    unsigned short max;

    MinMax(){};

    MinMax(unsigned short depth){
        min = depth;
        max = depth;
    }

    MinMax(MinMax tl, MinMax tr, MinMax bl, MinMax br) {
        min = std::min({tl.min, tr.min, bl.min, br.min});
        max = std::max({tl.max, tr.max, bl.max, br.max});
    }

    unsigned short decodedValue(){return max;};

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & max;
    }
};

using RollingQT32bitMinMaxAbsDiff = RollingQuadTree<NodeAddress32bit, MinMax, AbsDiffPolicy>;
using RollingQT16bitMinMaxAbsDiff = RollingQuadTree<NodeAddress16bit, MinMax, AbsDiffPolicy>;





#endif //DEPTHCODEC_QUADTREETYPES_H

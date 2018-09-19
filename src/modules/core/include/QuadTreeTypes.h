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
#include "ImageError.h"

class AbsDiffPolicy{
    short mThresh;
protected:
    template<typename LEAF_DATA_TYPE>
    bool shouldPrune(LEAF_DATA_TYPE leafData){
        //if(leafData.getMin() == 0)
        //    return true;
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

class PSNRPolicy{
    float mThresh;
protected:
    template<typename LEAF_DATA_TYPE>
    bool shouldPrune(LEAF_DATA_TYPE leafData){
        //if(leafData.getMin() == 0)
        return leafData.renderPSNR() <= mThresh;
    }

    PSNRPolicy(){};
public:
    PSNRPolicy(float thresh):mThresh(thresh){}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
    }

    friend boost::serialization::access;
};

///TODO roi should be replaced with a general version of region allowing for octtrees
template<typename T>
class RangeLeaf{
    Range<T> range;
    cv::Rect  mRoi;
    cv::Mat mUnderlyingDepth;
public:

    RangeLeaf(){};
    RangeLeaf(const cv::Mat& im, const cv::Rect& roi):mUnderlyingDepth(im), mRoi(roi){
        double min, max;
        if(roi.width == 1){
            auto depth = im.at<unsigned short>(roi.y, roi.x);
            range = Range<T>(depth, depth);
            return;
        }
        cv::Mat subIm(im, roi);
        cv::minMaxIdx(subIm, &min, &max);
        range = Range<T>(min, max);
    }

    void render(cv::Mat& subImage){
        subImage.setTo(range.getMax());
    }

    RangeLeaf(const std::vector<RangeLeaf>& rangeLeafs):mUnderlyingDepth(rangeLeafs[0].mUnderlyingDepth), mRoi(rangeLeafs[0].mRoi)
    {
        std::vector<Range<T>> ranges;
        ranges.resize(rangeLeafs.size());
        std::transform(rangeLeafs.begin(), rangeLeafs.end(), ranges.begin(), [](const RangeLeaf& x)->Range<T>{return x.range;});
        range = Range<T>(ranges);
        mRoi.width *= 2;
        mRoi.height *= 2;
    }

    float renderPSNR(){
        cv::Mat compressed(mRoi.size(), mUnderlyingDepth.type());
        this->render(compressed);
        return  PSNR(mUnderlyingDepth(mRoi), compressed);
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // note, version is always the latest when saving
        ar & range;
    }

    friend class boost::serialization::access;
};

using RollingQT32bitMinMaxAbsDiff = RollingQuadTree<NodeAddress32bit, RangeLeaf<unsigned short>, PSNRPolicy>;
using RollingQT16bitMinMaxAbsDiff = RollingQuadTree<NodeAddress16bit, RangeLeaf<unsigned short>, PSNRPolicy>;


#endif //DEPTHCODEC_QUADTREETYPES_H

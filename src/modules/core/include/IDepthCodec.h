//
// Created by philip on 8/8/18.
//

#ifndef DEPTHCODEC_COMPRESSIONQUADTREE_H
#define DEPTHCODEC_COMPRESSIONQUADTREE_H

#include <opencv2/opencv.hpp>
#include <boost/serialization/serialization.hpp>
#include "DataStream.hpp"

// A method for performing compression
class IDepthCodec{
	cv::Size mImageSize;
public:
    IDepthCodec() = default;

    virtual void compress(const cv::Mat& depthImage) = 0;
    virtual void decompress(cv::Mat& depthImage) = 0;

    void setImageSize(const cv::Size& imageSize){mImageSize = imageSize;}
    cv::Size getImageSize() const {return mImageSize;}

    /// returns true if the codec has a maximum size
    virtual bool getMaxImageSize(cv::Size& size) const{
        return false;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & mImageSize;
    }

    friend class boost::serialization::access;
};

#endif //DEPTHCODEC_COMPRESSIONQUADTREE_H

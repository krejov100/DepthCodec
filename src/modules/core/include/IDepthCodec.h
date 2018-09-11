//
// Created by philip on 8/8/18.
//

#ifndef DEPTHCODEC_COMPRESSIONQUADTREE_H
#define DEPTHCODEC_COMPRESSIONQUADTREE_H

#include <opencv2/opencv.hpp>
#include <CompressedData.h>
#include <boost/serialization/serialization.hpp>

// A method for performing compression
class IDepthCodec{
public:
    IDepthCodec() = default;

    virtual void compress(const cv::Mat& depthImage) = 0;
    virtual void decompress(cv::Mat& depthImage) = 0;

    virtual cv::Size getOptimalSize() = 0;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) const
    {
    }

    friend class boost::serialization::access;
};

#endif //DEPTHCODEC_COMPRESSIONQUADTREE_H

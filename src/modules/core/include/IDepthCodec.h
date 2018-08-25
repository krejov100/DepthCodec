//
// Created by philip on 8/8/18.
//

#ifndef DEPTHCODEC_COMPRESSIONQUADTREE_H
#define DEPTHCODEC_COMPRESSIONQUADTREE_H

#include <opencv2/opencv.hpp>
#include <CompressedData.h>

// A method for performing compression
class IDepthCodec{
public:
    virtual CompressedData compress(const cv::Mat& depthImage) = 0;
    virtual void decompress(const CompressedData& data, cv::Mat& depthImage) = 0;

    virtual cv::Size getOptimalSize() = 0;
};

#endif //DEPTHCODEC_COMPRESSIONQUADTREE_H

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
    virtual void compress(std::ostream& stream, const cv::Mat& depthImage) = 0;
    virtual void decompress(std::istream& stream, cv::Mat& depthImage) = 0;

    virtual void writeCompressedData(std::ostream& out) = 0
    virtual void readCompressedData(std::istream& in) = 0

    virtual cv::Size getOptimalSize() = 0;
};

#endif //DEPTHCODEC_COMPRESSIONQUADTREE_H

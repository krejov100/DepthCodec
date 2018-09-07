//
// Created by philip on 8/24/18.
//

#ifndef DEPTHCODEC_CVHELPERS_H
#define DEPTHCODEC_CVHELPERS_H

#include <opencv2/core/types.hpp>
#include "iostream"

template<class T>
std::ostream& operator<<(std::ostream& os, const cv::Size& size) {
    os << size.width << size.height;
    return os;
}

template<class T>
std::istream& operator>>(std::istream& is, cv::Size& size) {
    is >> size.width >> size.height;
    return is;
}

namespace cv {
    template<class Archive>
    inline void serialize(Archive &ar, cv::Size &t, const unsigned int file_version) {
        ar & t.width;
        ar & t.height;
    }
}
#endif //DEPTHCODEC_CVHELPERS_H

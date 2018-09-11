//
// Created by philip on 8/24/18.
//

#ifndef DEPTHCODEC_CVHELPERS_H
#define DEPTHCODEC_CVHELPERS_H

#include <opencv2/core/types.hpp>
#include "iostream"

inline void write(std::ostream &os, const cv::Size &size) {
    os << size.width << size.height;
}


inline void read(std::istream &is, cv::Size &size) {
    is >> size.width >> size.height;
}

inline void write(std::ostream &os, const cv::Mat m) {
    size_t elem_size = m.elemSize();
    size_t elem_type = m.type();

    write(os, m.size());
    os << elem_size;
    os << elem_type;

    const size_t data_size = m.size().area() * elem_size;
    os.write(reinterpret_cast<const char *>(m.data), data_size);
}

inline void read(std::istream &is, cv::Mat m) {
    cv::Size size;
    size_t elem_size, elem_type;

    read(is, size);
    is >> elem_size;
    is >> elem_type;

    m.create(size.height, size.width, elem_type);

    const size_t data_size = size.area() * elem_size;
    is.read(reinterpret_cast<char *>(m.data), data_size);
}

template<typename T>
void write(std::ostream &os, const std::vector<T> vec){
    size_t size = vec.size();
    //write size
    os.write((char*)&size, sizeof(size));
    //write vec
    for(auto& item : vec)
        write(os, item);
    os.write((char*)&vec[0], vec.size() * sizeof(T));
}

#endif //DEPTHCODEC_CVHELPERS_H

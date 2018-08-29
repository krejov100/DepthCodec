//
// Created by philip on 8/28/18.
//

#ifndef DEPTHCODEC_GRADIANT_H
#define DEPTHCODEC_GRADIANT_H

#include "iostream"

constexpr float computeVforN(const size_t n){
    return ((n*n*(n+1))/2);
}

constexpr float computeUforN(const size_t n){
    return ((n*n*(n+1)*(n+1))/4);
}

constexpr float computeTforN(const size_t n){
    return (n*n*(n+1)*((2*n)+1))/6;
}

//constexpr cv::gradiantMatrix(const size_t n);


#endif //DEPTHCODEC_GRADIANT_H

//
// Created by philip on 9/18/18.
//
#include <opencv2/opencv.hpp>
#include "ImageError.h"

///Mean Squared Error
double MSE(const cv::Mat& original, const cv::Mat& transformed){
    cv::Mat originalSigned;
    cv::Mat transformedSigned;

    original.convertTo(originalSigned, CV_32FC1);
    transformed.clone().convertTo(transformedSigned, CV_32FC1);

    originalSigned -= transformedSigned;
    cv::Mat squaredDiff;
    cv::multiply(originalSigned, originalSigned, squaredDiff);

    double summedSquaredDiff = cv::sum(squaredDiff)[0] / (originalSigned.rows * originalSigned.cols);

    return summedSquaredDiff;
}

///Peak Signal to Noise ratio
double PSNR(const cv::Mat& original, const cv::Mat& transformed){
    double mse = MSE(original, transformed);

    return (20 * log10(0xffff)) - (10 * log10(mse));
}
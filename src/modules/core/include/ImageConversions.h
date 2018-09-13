//
// Created by philip on 9/12/18.
//

#ifndef DEPTHCODEC_IMAGECONVERSIONS_H
#define DEPTHCODEC_IMAGECONVERSIONS_H

#include <librealsense2/hpp/rs_frame.hpp>
#include <Core/Geometry/Image.h>
#include "opencv2/opencv.hpp"

inline cv::Mat getOpenCVImage(const rs2::depth_frame& frame){
    // Query frame size (width and height)
    const int w = frame.get_width();
    const int h = frame.get_height();

    // Create OpenCV matrix of size (w,h) from the colorized depth data
    return cv::Mat(cv::Size(w, h), CV_16UC1, (void*)frame.get_data(), cv::Mat::AUTO_STEP);
}

inline cv::Mat getOpenCVImage(const rs2::frame& frame){
    // Query frame size (width and height)
    const int w = frame.as<rs2::video_frame>().get_width();
    const int h = frame.as<rs2::video_frame>().get_height();

    // Create OpenCV matrix of size (w,h) from the colorized depth data
    return cv::Mat(cv::Size(w, h), CV_8UC3, (void*)frame.get_data(), cv::Mat::AUTO_STEP);
}

inline three::Image getOpen3DImage(const cv::Mat& frame){
    three::Image image;
    auto type = frame.type();
    if(type == CV_16UC1)
        image.PrepareImage(frame.cols, frame.rows, 1, 2);
    else
        image.PrepareImage(frame.cols, frame.rows, 3, 1);
    image.data_ = std::vector<uint8_t>(frame.datastart, frame.dataend);
}

inline three::Image getOpen3DImage(const rs2::depth_frame& frame){
    auto data = frame.get_data();
    three::Image image;
    image.PrepareImage(frame.get_width(), frame.get_height(), 1, 2);
    image.data_ = std::vector<uint8_t>((uint8_t*)data, (uint8_t*)data + (frame.get_height() * frame.get_width() / sizeof(uint8_t)*2));
    return image;
}

inline three::Image getOpen3DImage(const rs2::frame& frame){
    auto data = frame.get_data();
    three::Image image;
    image.PrepareImage(frame.as<rs2::video_frame>().get_width(), frame.as<rs2::video_frame>().get_height(), 3, 1);
    image.data_ = std::vector<uint8_t>((uint8_t*)data, (uint8_t*)data + (frame.as<rs2::video_frame>().get_height() * frame.as<rs2::video_frame>().get_width() / sizeof(uint8_t)));
    return image;
}


#endif //DEPTHCODEC_IMAGECONVERSIONS_H

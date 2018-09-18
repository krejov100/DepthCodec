//
// Created by philip on 9/18/18.
//

#ifndef DEPTHCODEC_POINTCLOUDCONVERSIONS_H
#define DEPTHCODEC_POINTCLOUDCONVERSIONS_H

#include "opencv2/opencv.hpp"
#include "ImageConversions.h"
#include <Core/Core.h>
#include <librealsense2/h/rs_types.h>
#include <librealsense2/hpp/rs_frame.hpp>


inline std::shared_ptr<open3d::PointCloud> getOpen3dPointCloud(const cv::Mat& depth, const rs2_intrinsics& intrin,  const cv::Mat& color = cv::Mat()){
    auto depthImage = getOpen3DImage(depth);
    //auto colorImage = getOpen3DImage(color);

    //auto RGBImage = open3d::CreateRGBDImageFromColorAndDepth(depthImage, colorImage);
    ///PinholeCameraIntrinsic(int width, int height, double fx, double fy, double cx, double cy);
    return open3d::CreatePointCloudFromDepthImage(depthImage, open3d::PinholeCameraIntrinsic(
            depthImage.width_, depthImage.height_, intrin.fx, intrin.fy, intrin.ppx, intrin.ppy));
}

inline std::shared_ptr<open3d::PointCloud> getOpen3dPointCloud(const rs2::depth_frame& depth, const rs2_intrinsics intrin, const rs2::frame& color){
    auto depthImage = getOpen3DImage(depth);
    //auto colorImage = getOpen3DImage(color);

    //auto RGBImage = open3d::CreateRGBDImageFromColorAndDepth(depthImage, colorImage);
    ///PinholeCameraIntrinsic(int width, int height, double fx, double fy, double cx, double cy);
    //return open3d::CreatePointCloudFromRGBDImage(*RGBImage, open3d::PinholeCameraIntrinsic(
    //        depth.get_width(), depth.get_height(), intrin.fx, intrin.fy, intrin.ppx, intrin. ppy));
    return open3d::CreatePointCloudFromDepthImage(depthImage, open3d::PinholeCameraIntrinsic(
            depthImage.width_, depthImage.height_, intrin.fx, intrin.fy, intrin.ppx, intrin. ppy));
}


#endif //DEPTHCODEC_POINTCLOUDCONVERSIONS_H

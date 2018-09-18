//
// Created by philip on 9/18/18.
//

#ifndef DEPTHCODEC_FRAME_H
#define DEPTHCODEC_FRAME_H

#include "opencv2/opencv.hpp"
#include "librealsense2/rs.hpp"
#include "ImageConversions.h"
#include "PointCloudConversions.h"

class Frame {
    rs2::pointcloud mPc;
    cv::Mat mDepth;
    cv::Mat mColor;
    rs2_intrinsics mIntrinsics;
public:

    Frame(const rs2::frameset& fs){
        mDepth = getOpenCVImage(fs.get_depth_frame());
        // todo recompile LibRS removing the need for the const cast
        //mColor = getOpenCVImage(const_cast<rs2::frameset*>(&fs)->get_color_frame());
        mIntrinsics = fs.get_profile().as<rs2::video_stream_profile>().get_intrinsics();
    };

    cv::Mat getDepthImage() const
    {
        return mDepth;
    }

    void updateDepthImage(cv::Mat& depthImage){
        mDepth = depthImage.clone();
    }

    std::shared_ptr<open3d::PointCloud> getPointCloud() const
    {
        auto intrin = getIntrin();
        return getOpen3dPointCloud(mDepth, intrin);
    }

    rs2_intrinsics getIntrin() const
    {
        return mIntrinsics;
    }

    friend class FrameSource;
};


// RS Camera or ROS bag file
class FrameSource{
    rs2::context mContext;
    rs2::config mConfig;
    rs2::pipeline mPipe;
public:

    FrameSource(std::string bagFilePath):mContext(), mConfig(), mPipe(mContext) {
        mConfig.enable_device_from_file(bagFilePath);
        mPipe.start(mConfig);
    }

    Frame grabFrame(){
        return Frame(mPipe.wait_for_frames());
    }
};

#endif //DEPTHCODEC_FRAME_H

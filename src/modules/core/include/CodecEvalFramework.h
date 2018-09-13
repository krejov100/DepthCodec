//
// Created by philip on 8/2/18.
//

#pragma once
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <Visualization/Visualization.h>
#include <Geometry/Geometry.h>
#include <PinholeCameraIntrinsic.h>
#include <Core/Core.h>
#include "NamedTimer.h"
#include "librealsense2/rs.hpp"
#include "opencv2/opencv.hpp"
#include "DataStream.hpp"
#include "iostream"
#include "ImageConversions.h"

/// This is an interface for a compressable object
/// differnt datatypes will need to be compressed in differnt ways

/// template to IMAGE_TYPE with concept of image
double MSE(const cv::Mat& original, const cv::Mat& transformed);
double PSNR(const cv::Mat& original, const cv::Mat& transformed);
void showCompressionArtifacts(const cv::Mat& original, const cv::Mat& compressed);

class CompressionMetric{
public:
    CompressionMetric(){}
    NamedTimer compressionTimer;
    NamedTimer decompressionTimer;
    size_t originalSizeInBytes = 1;
    size_t compressedSizeInBytes = 1;
    float meanSquaredError = FLT_MAX;
    float peakSignalToNoise = 0;

    void printPerformance(std::ostream& ostream) const;
};

std::shared_ptr<three::PointCloud> PointCloud(const cv::Mat& depth, const rs2_intrinsics& intrin,  const cv::Mat& color = cv::Mat()){

}

std::shared_ptr<three::PointCloud> PointCloud(const rs2::depth_frame& depth, const rs2_intrinsics intrin, const rs2::frame& color){
    auto depthImage = getOpen3DImage(depth);
    auto colorImage = getOpen3DImage(color);

    auto RGBImage = three::CreateRGBDImageFromColorAndDepth(depthImage, colorImage);
    ///PinholeCameraIntrinsic(int width, int height, double fx, double fy, double cx, double cy);
    return three::CreatePointCloudFromRGBDImage(*RGBImage, three::PinholeCameraIntrinsic(
            depth.get_width(), depth.get_height(), intrin.fx, intrin.fy, intrin.ppx, intrin. ppy));
}

class Frame {
    rs2::frameset mFrameSet;
    rs2::pointcloud mPc;
    cv::Mat mDepth;
    cv::Mat mColor;
public:
    Frame(const rs2::frameset& fs) : mFrameSet(fs){
        mDepth = getOpenCVImage(fs.get_depth_frame());
        mColor = getOpenCVImage(fs.get_color_frame());
    };
    cv::Mat GetDepthImage();

    void SetDepthImage();

    std::shared_ptr<three::PointCloud> GetPointCloud(){


        auto intrin = getIntrin();

        return PointCloud(depth, intrin, color);
    }

    rs2_intrinsics getIntrin()
    {
        return mFrameSet.get_profile().as<rs2::video_stream_profile>().get_intrinsics();
    }

    friend class FrameSource;
};

// RS Camera or ROS bag file
class FrameSource{
    rs2::config mConfig;
    rs2::pipeline mPipe;
    rs2::context mContext;
public:
    FrameSource(std::string bagFilePath){
        mConfig.enable_device_from_file(bagFilePath);
        mPipe.start(mConfig);
        mContext = rs2::context();
    }

    Frame grabFrame(){
        return {mPipe.wait_for_frames()};
    }
};

// The CODEC_POLICY is templated as it may be depth based, requiring DATA_TYPE to be cv::Mat
// if its pointcloud based then it needs to be pcl::Pointcloud or open3d::Pointcloud
template<typename DATA_TYPE, typename CODEC_TYPE>
class CodecEvalFramework{
    std::vector<CompressionMetric> results;
    CODEC_TYPE* mCodec;
public:

    CodecEvalFramework(CODEC_TYPE* codec):mCodec(codec){};

    CompressionMetric evaluateCodecOnExample(const DATA_TYPE& example, bool showArtifacts=true)
    {
        CompressionMetric rslt;
        rslt.originalSizeInBytes = 0;

        /// Evaluate Compression
        rslt.compressionTimer = NamedTimer("Compression");
        std::stringstream ss;
        boost::archive::binary_oarchive bo(ss);
        boost::archive::binary_iarchive bi(ss);
        mCodec->compress(example);
        rslt.compressionTimer.endTimer();

        bo << mCodec;
        /// Evaluate CompressedData https://stackoverflow.com/questions/4432793/size-of-stringstream
        rslt.compressedSizeInBytes = ss.str().size();
        bi >> mCodec;

        /// Evaluate decompression
        rslt.decompressionTimer = NamedTimer("Decompression");
        DATA_TYPE decompressed;
        mCodec->decompress(decompressed);
        rslt.decompressionTimer.endTimer();

        /// Evaluate lossyness
        //rslt.meanSquaredError = MSE(example, decompressed);
        //rslt.peakSignalToNoise = PSNR(example, decompressed);
        //if(showArtifacts)
         //   showCompressionArtifacts(example, decompressed);

        return rslt;
    };

    FrameSource loadRosBag(std::string)
};

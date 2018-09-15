//
// Created by philip on 8/2/18.
//

#pragma once
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <Core/Core.h>
#include <IO/IO.h>
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
#include "DepthCodec.h"


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

inline std::shared_ptr<open3d::PointCloud> getOpen3dPointCloud(const cv::Mat& depth, const rs2_intrinsics& intrin,  const cv::Mat& color = cv::Mat()){
    auto depthImage = getOpen3DImage(depth);
    //auto colorImage = getOpen3DImage(color);

    //auto RGBImage = three::CreateRGBDImageFromColorAndDepth(depthImage, colorImage);
    ///PinholeCameraIntrinsic(int width, int height, double fx, double fy, double cx, double cy);
    return open3d::CreatePointCloudFromDepthImage(depthImage, open3d::PinholeCameraIntrinsic(
            depthImage.width_, depthImage.height_, intrin.fx, intrin.fy, intrin.ppx, intrin.ppy));
}

inline std::shared_ptr<open3d::PointCloud> getOpen3dPointCloud(const rs2::depth_frame& depth, const rs2_intrinsics intrin, const rs2::frame& color){
    auto depthImage = getOpen3DImage(depth);
    //auto colorImage = getOpen3DImage(color);

    //auto RGBImage = three::CreateRGBDImageFromColorAndDepth(depthImage, colorImage);
    ///PinholeCameraIntrinsic(int width, int height, double fx, double fy, double cx, double cy);
    //return three::CreatePointCloudFromRGBDImage(*RGBImage, three::PinholeCameraIntrinsic(
    //        depth.get_width(), depth.get_height(), intrin.fx, intrin.fy, intrin.ppx, intrin. ppy));
    return open3d::CreatePointCloudFromDepthImage(depthImage, open3d::PinholeCameraIntrinsic(
            depthImage.width_, depthImage.height_, intrin.fx, intrin.fy, intrin.ppx, intrin. ppy));
}

class Frame {
    rs2::pointcloud mPc;
    cv::Mat mDepth;
    cv::Mat mColor;
    rs2_intrinsics mIntrinsics;
public:

    Frame(const rs2::frameset& fs){
        mDepth = getOpenCVImage(fs.get_depth_frame());
        // todo recompile LibRS removing the need for the const cast
        mColor = getOpenCVImage(const_cast<rs2::frameset*>(&fs)->get_color_frame());
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
        return getOpen3dPointCloud(mDepth, intrin, mColor);
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


template<typename CODEC_TYPE, typename DATA_TYPE>
void compressAndDecompress(CODEC_TYPE& codec, const DATA_TYPE& example, DATA_TYPE& rslt)
{
    std::stringstream ss;
    boost::archive::binary_oarchive bo(ss);
    boost::archive::binary_iarchive bi(ss);
    codec->compress(example);
    bo << codec;
    bi >> codec;
    codec->decompress(rslt);
}

// TODO write function that evaluates against a ros bag file
inline FrameSource LoadRosBag(const std::string& path)
{
    return FrameSource(path);
};

// The CODEC_POLICY is templated as it may be depth based, requiring DATA_TYPE to be cv::Mat
// if its pointcloud based then it needs to be pcl::Pointcloud or open3d::Pointcloud
template<typename DATA_TYPE, typename CODEC_TYPE>
class CodecEvalFramework{
    std::vector<CompressionMetric> results;
    std::shared_ptr<CODEC_TYPE> mCodec;
public:

    CodecEvalFramework(std::shared_ptr<CODEC_TYPE> codec):mCodec(codec){};


    /// Todo Extract the visualisation to a seperate class
    CompressionMetric evaluateCodecOnExample(const DATA_TYPE& example, bool showArtifacts=true)
    {
        CompressionMetric rslt;
        rslt.originalSizeInBytes = example.total() * example.elemSize();

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
        rslt.meanSquaredError = MSE(example, decompressed);
        rslt.peakSignalToNoise = PSNR(example, decompressed);
        if(showArtifacts) {
            showCompressionArtifacts(example, decompressed);
            showCompressionArtifacts(example, decompressed);
        }
        cv::imshow("sdf", decompressed);

        return rslt;
    };

    CompressionMetric evaluateCodecOnPointCloud(std::shared_ptr<open3d::PointCloud> pc, bool showArtifacts=true){

        if(showArtifacts)
            open3d::DrawGeometries({pc});
    }
};

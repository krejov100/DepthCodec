//
// Created by philip on 8/2/18.
//

#pragma once
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <PinholeCameraIntrinsic.h>
#include "NamedTimer.h"
#include "opencv2/opencv.hpp"
#include "DataStream.hpp"
#include "iostream"
#include "DepthCodec.h"
#include "Frame.h"


/// This is an interface for a compressable object
/// differnt datatypes will need to be compressed in differnt ways

/// template to IMAGE_TYPE with concept of image
double MSE(const cv::Mat& original, const cv::Mat& transformed);
double PSNR(const cv::Mat& original, const cv::Mat& transformed);
void showCompressionArtifacts(const cv::Mat& original, const cv::Mat& compressed);
void showPointCloudCompression(const Frame& originalFrame, const Frame& compressedFrame);

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

    }
};

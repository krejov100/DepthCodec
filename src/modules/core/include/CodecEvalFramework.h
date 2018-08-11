//
// Created by philip on 8/2/18.
//

#ifndef CAPTURE_TRIANGULATION_COMPRESSION_H
#define CAPTURE_TRIANGULATION_COMPRESSION_H

#include <vector>
#include "NamedTimer.h"
#include "opencv2/opencv.hpp"
#include "CompressedData.h"

/// This is an interface for a compressable object
/// differnt datatypes will need to be compressed in differnt ways

#endif //CAPTURE_TRIANGULATION_COMPRESSION_H

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

// The CODEC_POLICY is templated as it may be depth based, requiring DATA_TYPE to be cv::Mat
// if its pointcloud based then it needs to be pcl::Pointcloud or open3d::Pointcloud
template<typename DATA_TYPE, typename CODEC_PTR_TYPE>
class CodecEvalFramework{
    std::vector<CompressionMetric> results;
    CODEC_PTR_TYPE* mCodec;
public:

    CodecEvalFramework(CODEC_PTR_TYPE* codec):mCodec(codec){};

    CompressionMetric evaluateCodecOnExample(const DATA_TYPE& example, bool showArtifacts=true) const
    {
        CompressionMetric rslt;
        rslt.originalSizeInBytes = 0;

        /// Evaluate Compression
        rslt.compressionTimer = NamedTimer("Compression");
        CompressedData compressedData = mCodec->compress(example);
        rslt.compressionTimer.endTimer();

        /// Evaluate CompressedData
        rslt.originalSizeInBytes = compressedData.size();

        /// Evaluate decompression
        rslt.decompressionTimer = NamedTimer("Decompression");
        DATA_TYPE decompressed;
        mCodec->decompress(compressedData, decompressed);
        rslt.decompressionTimer.endTimer();

        /// Evaluate lossyness
        rslt.meanSquaredError = MSE(example, decompressed);
        rslt.peakSignalToNoise = PSNR(example, decompressed);
        if(showArtifacts)
            showCompressionArtifacts(example, decompressed);

        return rslt;
    };
};


//
// Created by philip on 8/2/18.
//

#ifndef CAPTURE_TRIANGULATION_COMPRESSION_H
#define CAPTURE_TRIANGULATION_COMPRESSION_H

#include <vector>
#include "NamedTimer.h"
#include "opencv2/opencv.hpp"

/// This is an interface for a compressable object
/// differnt datatypes will need to be compressed in differnt ways

#endif //CAPTURE_TRIANGULATION_COMPRESSION_H

using CompressedData = typename std::vector<unsigned char>;

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

template<typename DATA_TYPE, typename CODEC_POLICY>
class CodecEvalFramework: private CODEC_POLICY{
    using CODEC_POLICY::compress;
    using CODEC_POLICY::decompress;

    std::vector<CompressionMetric> results;
public:

    CodecEvalFramework(){};

    CompressionMetric evaluateCodecOnExample(const DATA_TYPE& example, bool showArtifacts=true) const
    {
        CompressionMetric rslt;
        rslt.originalSizeInBytes = 0;

        /// Evaluate Compression
        rslt.compressionTimer = NamedTimer("Compression");
        CompressedData compressedData = compress(example);
        rslt.compressionTimer.endTimer();

        /// Evaluate CompressedData
        rslt.originalSizeInBytes = compressedData.size();

        /// Evaluate decompression
        rslt.decompressionTimer = NamedTimer("Decompression");
        DATA_TYPE decompressed;
        decompress(compressedData, decompressed);
        rslt.decompressionTimer.endTimer();

        /// Evaluate lossyness
        rslt.meanSquaredError = MSE(example, decompressed);
        rslt.peakSignalToNoise = PSNR(example, decompressed);
        if(showArtifacts)
            showCompressionArtifacts(example, decompressed);

        return rslt;
    };
};


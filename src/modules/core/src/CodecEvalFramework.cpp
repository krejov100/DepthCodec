//
// Created by philip on 8/3/18.
//

#include "CodecEvalFramework.h"
#include "opencv2/opencv.hpp"
#include <Visualization/Visualization.h>

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

void showDifferanceImage(const cv::Mat& original, const cv::Mat& transformed){
    cv::Mat originalSigned;
    cv::Mat transformedSigned;

    original.convertTo(originalSigned, CV_32FC1);
    transformed.clone().convertTo(transformedSigned, CV_32FC1);

    transformedSigned -= originalSigned;
    cv::Mat diff8Bit;

    transformedSigned.convertTo(diff8Bit, CV_8UC1, 255.0/65536.0, 255/2);
    cv::Mat colorDiffImage;
    cv::applyColorMap(diff8Bit, colorDiffImage, cv::COLORMAP_JET);
    cv::namedWindow("Difference Image", cv::WINDOW_NORMAL | cv::WINDOW_GUI_EXPANDED );
    cv::imshow("Difference Image", colorDiffImage);
}

void showPointCloudCompression(const Frame& originalFrame, const Frame& compressedFrame){

    auto originalPC = originalFrame.getPointCloud();
    auto compressedPC = compressedFrame.getPointCloud();
    originalPC->PaintUniformColor({1, 0.706, 0});
    compressedPC->PaintUniformColor({0, 0.651, 0.929});

    open3d::DrawGeometries({originalPC, compressedPC});
}

void showCompressionArtifacts(const cv::Mat& original, const cv::Mat& compressed){
    showDifferanceImage(original, compressed);
}

void CompressionMetric::printPerformance(std::ostream& ostream) const
{
    float GBperHour = 30.0f * 60.0f * 60.0f / 1000.0f / 1000.0f / 1000.0f;

    ostream << "Original:\t" << originalSizeInBytes / 1000 << "KB" <<
    ",\t1 hour rec: " << originalSizeInBytes * GBperHour << "GB" << std::endl;

    ostream << "Compressed size:\t" << compressedSizeInBytes/1000 << "KB" <<
    ",\t1 hour rec: " << compressedSizeInBytes * GBperHour << "GB" << std::endl;

    ostream << "Compression ratio: " << originalSizeInBytes / float(compressedSizeInBytes) << std::endl;

    compressionTimer.printTimer(ostream);
    decompressionTimer.printTimer(ostream);
    ostream << "Time asymmetry compression/decompression: " << compressionTimer.getTime() / decompressionTimer.getTime() << std::endl;

    ostream << "Compression MSE: " << meanSquaredError << " PSNR: " << peakSignalToNoise << std::endl;

}
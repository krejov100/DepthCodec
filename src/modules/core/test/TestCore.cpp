#define BOOST_TEST_MODULE TestCore
#include <boost/test/included/unit_test.hpp>
#include <boost/log/trivial.hpp>
#include "opencv2/opencv.hpp"
#include "NodeAddress.h"
#include "RollingQuadTree.h"
#include "CodecEvalFramework.h"

BOOST_AUTO_TEST_CASE(TestTest){
    std::cout << "Using Opencv "
              << cv::getVersionString()
              << std::endl;

    std::cout << "Using Boost "
              << BOOST_VERSION / 100000     << "."  // major version
              << BOOST_VERSION / 100 % 1000 << "."  // minor version
              << BOOST_VERSION % 100                // patch level
              << std::endl;
}

//return the depth in the tree given an address
BOOST_AUTO_TEST_CASE(TestQuadTreeGetDepth) {
    NodeAddress address = 0b0000100101110100;
    BOOST_TEST(getDepth(address) == 4);
    address = 0b0000000101110100;
    BOOST_TEST(getDepth(address) == 3);
    address = 0b0000000111111111;
    BOOST_TEST(getDepth(address) == 3);
}

/// given an address, return the tl tr bl br of that address
BOOST_AUTO_TEST_CASE(TestQuadTreeSiblings) {
    NodeAddress address = 0b0000101101110100;

    auto pa = parentAddress(address);
    auto tl = topLeft(pa);
    auto tr = topRight(pa);
    auto bl = bottomLeft(pa);
    auto br = bottomRight(pa);

    /// tl tr bl br in Z-order   https://en.wikipedia.org/wiki/Z-order_curve
    printAddress(tl);
    BOOST_TEST(tl == 0b0000100101110100);
    BOOST_TEST(tr == 0b0000101101110100);
    BOOST_TEST(bl == 0b0000110101110100);
    BOOST_TEST(br == 0b0000111101110100);
}


class AbsDiffPolicy{
    short mThresh;
protected:
    template<typename LEAF_DATA_TYPE>
    bool shouldPrune(LEAF_DATA_TYPE leafData){
        //if(leafData.min == 0)
        //    return true;
        return abs(leafData.min-leafData.max) <= mThresh;
    }
public:
    AbsDiffPolicy(short thresh):mThresh(thresh){}
};


class MinMax{
public:
    unsigned short min;
    unsigned short max;

    MinMax(){};

    MinMax(unsigned short depth){
        min = depth;
        max = depth;
    }

    MinMax(MinMax tl, MinMax tr, MinMax bl, MinMax br) {
        min = std::min({tl.min, tr.min, bl.min, br.min});
        max = std::max({tl.max, tr.max, bl.max, br.max});
    }

    unsigned short decodedValue(){return max;};
};

BOOST_AUTO_TEST_CASE(TestTreeBuild){
    NodeAddress address = 0b000100;
    auto pa = parentAddress(address);
    BOOST_TEST(pa == 0b000);

    RollingQuadTree<MinMax, AbsDiffPolicy> t(AbsDiffPolicy(4));
}

BOOST_AUTO_TEST_CASE(TestEncodeAddressRecurse) {
    auto a = encodeAddressRecurse(3, 2, 4/2, 4/2, 2);
    printAddress(a);
    BOOST_TEST(a == 0b101111);


    auto b = encodeAddressRecurse(199, 199, 200/2, 200/2, 6);
    printAddress(b);
    BOOST_TEST(b == 0b111111111111111111);


    auto c = encodeAddressRecurse(0, 0, 200/2, 200/2, 6);
    printAddress(c);
    BOOST_TEST(c == 0b100100100100100100);

    auto d = encodeAddressRecurse(1, 2, 4/2, 4/2, 2);
    printAddress(d);
    BOOST_TEST(d == 0b101110);
}

BOOST_AUTO_TEST_CASE(TestDecodeAddressRecurse) {
    auto a = decodeAddressRecurse(0b101111, 0, 0, 4, 4);
    BOOST_TEST(std::get<0>(a) == 3);
    BOOST_TEST(std::get<1>(a) == 2);
    BOOST_TEST(std::get<0>(a) == 3);
    BOOST_TEST(std::get<1>(a) == 2);


    auto b = decodeAddressRecurse(0b111111111111111111111111, 0, 0, 256, 256);
    BOOST_TEST(std::get<0>(b) == 255);
    BOOST_TEST(std::get<1>(b) == 255);

    auto c = decodeAddressRecurse(0b100100100100100100, 0, 0, 256, 256);
    BOOST_TEST(std::get<0>(c) == 0);
    BOOST_TEST(std::get<1>(c) == 0);

    auto d = decodeAddressRecurse(0b101110, 0, 0, 4, 4);
    BOOST_TEST(std::get<0>(d) == 1);
    BOOST_TEST(std::get<1>(d) == 2);

    auto e = decodeAddressRecurse(0b101110, 0, 0, 4, 4);
    BOOST_TEST(std::get<0>(e) == 1);
    BOOST_TEST(std::get<1>(e) == 2);

    auto f = decodeAddressRecurse(0b101110, 0, 0, 4, 4);
    BOOST_TEST(std::get<2>(f) == 1);
    BOOST_TEST(std::get<3>(f) == 1);

    auto g = decodeAddressRecurse(0b000110, 0, 0, 4, 4);
    BOOST_TEST(std::get<2>(g) == 2);
    BOOST_TEST(std::get<3>(g) == 2);

}



BOOST_AUTO_TEST_CASE(TestPerfectEncodeDecode){
    NodeAddress address = 0b000100;
    auto pa = parentAddress(address);
    BOOST_TEST(pa == 0b000);

    cv::Mat example = cv::imread("../../modules/core/resources/bgExampleDepth.tif");
    cv::resize(example, example, cv::Size(1024,720));

    cv::Mat gray_image;
    cv::cvtColor(example, gray_image, CV_BGR2GRAY);
    cv::Mat shortMat;
    gray_image.convertTo(shortMat, CV_16UC1, 65536/255);


    RollingQuadTree<MinMax, AbsDiffPolicy> t(shortMat, AbsDiffPolicy(30 * (65536/255)));

    auto decompressed = t.decode();
    showCompressionArtifacts(shortMat, decompressed);
    std::vector<uchar> exampleVec(example.datastart,  example.dataend);
    std::vector<uchar> decompressedVec(decompressed.datastart,  decompressed.dataend);

    /*BOOST_TEST(exampleVec.size() == decompressedVec.size());
    for(size_t i = 0; i < exampleVec.size(); i++)
        BOOST_TEST(exampleVec[i] == decompressedVec[i]);*/



    cv::imshow("decompressed", decompressed);
    cv::waitKey(0);
}

class MockLossyhCodec{
    mutable cv::Mat original;
protected:

    CompressedData compress(const cv::Mat& data) const {
        original = data;
        BOOST_LOG_TRIVIAL(info) << "Compressing Data";
        return CompressedData();
    };


    void decompress(const CompressedData& compressedData, cv::Mat& data)const {
        data = original;
    };

};

BOOST_AUTO_TEST_CASE(TestCompression){
    CodecEvalFramework<cv::Mat, MockLossyhCodec> testFramework;
    auto rslt = testFramework.evaluateCodecOnExample(cv::Mat(640, 480, CV_16UC1, cv::Scalar(0)), false);
    rslt.printPerformance(std::cout);
}

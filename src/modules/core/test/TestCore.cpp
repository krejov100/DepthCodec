#define BOOST_TEST_MODULE TestCore
#include <boost/test/included/unit_test.hpp>
#define BOOST_LOG_DYN_LINK 1
#include <boost/log/trivial.hpp>
#include <DepthCodecFactory.h>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "opencv2/opencv.hpp"
#include "CodecEvalFramework.h"
#include "NodeAddress.h"
#include "QuadTreeTypes.h"
#include "QuadTreeCodecFactory.h"
#include "opencv2/core/version.hpp"
#include "ReadWrite.h"
#include "Range.h"
#include "BoostMarshaller.hpp"



BOOST_CLASS_EXPORT(RollingQT32bitMinMaxAbsDiff);
BOOST_CLASS_EXPORT(RollingQT16bitMinMaxAbsDiff);
BOOST_CLASS_EXPORT_GUID(IDepthCodec, "IDepthCodec");
BOOST_CLASS_EXPORT_GUID(TiledCodec,"TiledCodec");

BOOST_AUTO_TEST_CASE(TestMapStream)
{
    std::map<int, std::string> test{ {2, "hello"}, {3,"world" } }, rslt;
	BoostMarshaller<std::map<int, std::string>> bm;
    auto data = bm.marshall(test);
    bm.unmarshall(data);

	BOOST_TEST(test == rslt);
}

BOOST_AUTO_TEST_CASE(TestTest){
    std::cout << "Using OpenCV "
              << CV_VERSION_MAJOR << "."  // major version
              << CV_VERSION_MINOR << "."  // minor version
              << CV_VERSION_REVISION    // patch level
              << std::endl;

    std::cout << "Using Boost "
              << BOOST_VERSION / 100000     << "."  // major version
              << BOOST_VERSION / 100 % 1000 << "."  // minor version
              << BOOST_VERSION % 100                // patch level
              << std::endl;
}

//return the depth in the tree given an address
BOOST_AUTO_TEST_CASE(TestQuadTreeGetDepth) {
    NodeAddress32bit address = 0b0000100101110100;
    BOOST_TEST(getDepth(address) == 4);
    address = 0b0000000101110100;
    BOOST_TEST(getDepth(address) == 3);
    address = 0b0000000111111111;
    BOOST_TEST(getDepth(address) == 3);
}

/// given an address, return the tl tr bl br of that address
BOOST_AUTO_TEST_CASE(TestQuadTreeSiblings) {
    NodeAddress32bit address = 0b0000101101110100;

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


BOOST_AUTO_TEST_CASE(TestEncodeAddressRecurse) {
    auto a = encodeAddressRecurse<NodeAddress32bit>(3, 2, 4/2, 4/2, 2);
    printAddress(a);
    BOOST_TEST(a == 0b101111);

    auto b = encodeAddressRecurse<NodeAddress32bit>(199, 199, 200/2, 200/2, 6);
    printAddress(b);
    BOOST_TEST(b == 0b111111111111111111);


    auto c = encodeAddressRecurse<NodeAddress32bit>(0, 0, 200/2, 200/2, 6);
    printAddress(c);
    BOOST_TEST(c == 0b100100100100100100);

    auto d = encodeAddressRecurse<NodeAddress32bit>(1, 2, 4/2, 4/2, 2);
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
    NodeAddress32bit address = 0b000100;
    auto pa = parentAddress(address);
    BOOST_TEST(pa == 0b000);

    cv::Mat example = cv::imread("../../modules/core/resources/bgExampleDepth.tif");
    cv::resize(example, example, cv::Size(512,512), cv::INTER_NEAREST);

    cv::Mat gray_image;
    cv::cvtColor(example, gray_image, CV_BGR2GRAY);
    cv::Mat shortMat;
    gray_image.convertTo(shortMat, CV_16UC1, 65536/255);

    using Codec = RollingQuadTree<NodeAddress32bit, Range<unsigned short>, AbsDiffPolicy>;
    Codec t(AbsDiffPolicy(30 * (65536/255)));

    BoostMarshaller<Codec> bm;
    t.compress(shortMat);
    auto data = bm.marshall(t);
    t = bm.unmarshall(data);
    cv::Mat decompressed(512, 512, CV_16UC1, cv::Scalar(0));
    t.decompress(decompressed);
    showCompressionArtifacts(shortMat, decompressed);
    std::vector<uchar> exampleVec(example.datastart,  example.dataend);
    std::vector<uchar> decompressedVec(decompressed.datastart,  decompressed.dataend);

    /*BOOST_TEST(exampleVec.size() == decompressedVec.size());
    for(size_t i = 0; i < exampleVec.size(); i++)
        BOOST_TEST(exampleVec[i] == decompressedVec[i]);*/

    cv::imshow("decompressed", decompressed);
    cv::waitKey(0);
}

class MockLossyCodec:public IDepthCodec{
    cv::Mat mImage;

public:
    MockLossyCodec(){};

    void compress(const cv::Mat& data){
        mImage = data.clone();
        BOOST_LOG_TRIVIAL(info) << "Compressing Data";
    };

    void decompress(cv::Mat& data){
        data = mImage;
    };


    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // note, version is always the latest when saving
        boost::serialization::void_cast_register<MockLossyCodec, IDepthCodec>();
        ar & mImage;
    }

    friend class boost::serialization::access;
};

BOOST_AUTO_TEST_CASE(TestCodecFramework){
    auto codec = std::make_shared<MockLossyCodec>();
    CodecEvalFramework<cv::Mat, MockLossyCodec> testFramework(codec);
    auto rslt = testFramework.evaluateCodecOnExample(cv::Mat(640, 480, CV_16UC1, cv::Scalar(0)), false);
    rslt.printPerformance(std::cout);
}

BOOST_AUTO_TEST_CASE(TestPlayRosBag){
    FrameSource fs("/home/philip/Downloads/structured.bag");

    auto frame = fs.grabFrame();
    frame.getIntrin();
}

BOOST_AUTO_TEST_CASE(TestCodecFactory){
    cv::Mat example = cv::imread("../../modules/core/resources/bgExampleDepth.tif");

    cv::Mat gray_image;
    cv::cvtColor(example, gray_image, CV_BGR2GRAY);
    cv::Mat shortMat;
    gray_image.convertTo(shortMat, CV_16UC1, 65536/255);

    cv::resize(shortMat, shortMat, cv::Size(1280,720));

    boost::program_options::variables_map compressionOptions;
    compressionOptions.insert(std::make_pair("CodecType",po::variable_value(std::string("QuadTree"), false)));
    po::notify(compressionOptions);
    auto codecFactory = std::make_shared<DepthCodecFactory>(compressionOptions);
    codecFactory->registerSubFactory("QuadTree", std::make_shared<QuadTreeCodecFactory>(compressionOptions));

    auto codec = std::make_shared<TiledCodec>(codecFactory);

    CodecEvalFramework<cv::Mat, IDepthCodec> testFramework(codec);
    auto rslt = testFramework.evaluateCodecOnExample(shortMat, true);
    rslt.printPerformance(std::cout);

    cv::waitKey(0);
}

BOOST_AUTO_TEST_CASE(TestLoadRosBag){
    auto fs = LoadRosBag("/home/philip/Downloads/stairs.bag");

    auto frame = fs.grabFrame();

    boost::program_options::variables_map compressionOptions;
    compressionOptions.insert(std::make_pair("CodecType", po::variable_value(std::string("QuadTree"), false)));
    po::notify(compressionOptions);
    auto codecFactory = std::make_shared<DepthCodecFactory>(compressionOptions);
    codecFactory->registerSubFactory("QuadTree", std::make_shared<QuadTreeCodecFactory>(compressionOptions));

    auto codec = std::make_shared<TiledCodec>(codecFactory);

    cv::Mat depthImage = frame.getDepthImage();
    cv::Mat rslt;

    compressAndDecompress(codec, depthImage, rslt);

    frame.updateDepthImage(rslt);
    open3d::Visualizer vis;

    open3d::DrawGeometries({frame.getPointCloud()});
    //vis.AddGeometry(frame.getPointCloud());
    //vis.UpdateGeometry();

    //cv::waitKey(0);
}


BOOST_AUTO_TEST_CASE(TestCompressionFactory){
    std::cout<< DepthCodecFactory().getOptions();


}


//BOOST_CLASS_EXPORT(Tree)

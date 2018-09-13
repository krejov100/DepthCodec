//
// Created by philip on 8/10/18.
//

#ifndef DEPTHCODEC_MULTIPLEQUADTREE_H
#define DEPTHCODEC_MULTIPLEQUADTREE_H


#include <memory>
#include "DepthCodecFactory.h"

/// class that holds multiple codecs for a single image
class TiledCodec : public IDepthCodec
{
	std::vector<std::shared_ptr<IDepthCodec>> mSubCodecs;
	std::shared_ptr<DepthCodecFactory> mFactory;

protected:
	TiledCodec() {};
public:
	TiledCodec(std::shared_ptr<DepthCodecFactory> factory) :mFactory(factory) {};

	virtual void compress(const cv::Mat& depthImage)
	{
		this->setImageSize(depthImage.size());
		auto exampleCodec = mFactory->construct();
		cv::Size maxImageSize, imageSize;
		if (exampleCodec->getMaxImageSize(maxImageSize))
			imageSize = this->getImageSize();
		for (int y = 0; y < imageSize.height; y += maxImageSize.height)
		{
			for (int x = 0; x < imageSize.width; x += maxImageSize.width)
			{
				// use the max size of the tree for the width and height, or what remains of the image
				size_t width = std::min(maxImageSize.width, imageSize.width - x);
				size_t height = std::min(maxImageSize.height, imageSize.height - y);

				auto codec = mFactory->construct();
				cv::Rect subRegion(x, y, width, height);
				cv::Mat subMat = depthImage(subRegion);

				codec->compress(subMat);

				mSubCodecs.push_back(codec);
			}
		}
	}

	virtual void decompress(cv::Mat& depthImage)
	{
		depthImage = cv::Mat(getImageSize(), CV_16UC1, cv::Scalar(0));
		size_t x = 0, y = 0;
		for (auto& subCodec : mSubCodecs)
		{
			// get the size of the underlying codec and decompress that portion of the image
			cv::Size subImageSize;
			subImageSize = subCodec->getImageSize();

			cv::Rect subRegion(x, y, subImageSize.width, subImageSize.height);
			cv::Mat subMat = depthImage(subRegion);
			subCodec->decompress(subMat);

			if (subImageSize.width + x >= getImageSize().width)
			{
				x = 0;
				y += subImageSize.height;
			}
			else
				x += subImageSize.width;
			//move to the next portion of the image
		}
	}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<IDepthCodec>(*this);
		boost::serialization::void_cast_register<TiledCodec, IDepthCodec>();
		ar & mSubCodecs;
	}

	friend class boost::serialization::access;
};


/*
template<typename TREE_TYPE>
class MultipleQuadTreeCodec: public IDepthCodec
{
	/// This class uses multiple trees to encode an image, reasoning being that it can use a shorter address
	/// and not prune all the way to the top

	/// This class makes use of a prototype tree that has a configured set of parameters and replicates it accross the image
	TREE_TYPE mProtoTypeTree;
	size_t mImageWidth;
	size_t mImageHeight;

	/// the wrapped tree, now also includes an x, y position, describing which ROI it is responsible for parsing
	class ImageParsingQuadTreeWrapper
	{
		short mXPosition;
		short mYPosition;
		const std::shared_ptr<TREE_TYPE> mTree;

	public:
		ImageParsingQuadTreeWrapper(short xPosition, short yPosition, const std::shared_ptr<TREE_TYPE> tree):
				mXPosition(xPosition),
				mYPosition(yPosition),
				mTree(tree)
		{}

		virtual void parseImage(const cv::Mat & im) {mTree->parseImage(im);}
		virtual void reconstructImage(cv::Mat & rslt) const {mTree->reconstructImage(rslt);}
		short getXPosition(){return mXPosition;}
		short getYPosition(){return mYPosition;}

		friend std::ostream& operator<<(std::ostream& os, const ImageParsingQuadTreeWrapper& dt);
		friend std::istream& operator>>(std::istream& is, ImageParsingQuadTreeWrapper& codec);
	};

	// vector of trees that combine to represent the image as a whole
	std::vector<std::shared_ptr<ImageParsingQuadTreeWrapper>> mSubRegionTrees;

protected:
	void addTree(short xPosition, short yPosition, std::shared_ptr<TREE_TYPE> tree)
	{
		mSubRegionTrees.push_back(std::make_shared<ImageParsingQuadTreeWrapper>(xPosition, yPosition, tree));
	}

public:

	MultipleQuadTreeCodec(TREE_TYPE prototypeTree):mProtoTypeTree(prototypeTree){
		cv::namedWindow("test", cv::WINDOW_NORMAL | cv::WINDOW_GUI_EXPANDED | cv::WINDOW_KEEPRATIO);
	};

	void parseImage(const cv::Mat & im) {

	};

	void reconstructImage(cv::Mat& rslt)const {
		rslt = cv::Mat(mImageHeight, mImageWidth, CV_16UC1, cv::Scalar(0));
		for(auto& tree : mSubRegionTrees){
			// subregion of the image represented using a single tree
			short x = tree->getXPosition();
			short y = tree->getYPosition();
			size_t maxTreeSize = TREE_TYPE::getMaxDimensionLength();
			size_t width = std::min(maxTreeSize , mImageWidth - x);
			size_t height = std::min(maxTreeSize , mImageHeight - y);
			cv::Rect subRegion(x, y, width, height);
			std::cout << x << " " << y << " " << width << " " << height <<std::endl;
			cv::Mat subMat = rslt(subRegion);
			tree->reconstructImage(subMat);

			cv::imshow("test", rslt);
		}
	}

	virtual CompressedData compress(const cv::Mat& depthImage){
		parseImage(depthImage);
		return CompressedData();
	};

	virtual void decompress(const CompressedData& data, cv::Mat& depthImage){
		reconstructImage(depthImage);
	};
};

std::ostream& operator<<(std::ostream& os, const ImageParsingQuadTreeWrapper& codec)
{
	os << codec.mProtoTypeTree << codec.mImageWidth << codec.mImageHeight << codec.mSubRegionTrees;
	return os;
}

std::istream& operator>>(std::istream& is, ImageParsingQuadTreeWrapper& codec)
{
	is >> codec.mXPosition >> codec.mYPosition >> codec.mTree;
	return is;
}


*/
#endif //DEPTHCODEC_MULTIPLEQUADTREE_H

//
// Created by philip on 8/10/18.
//

#ifndef DEPTHCODEC_MULTIPLEQUADTREE_H
#define DEPTHCODEC_MULTIPLEQUADTREE_H


#include <memory>
#include "IDepthCodec.h"

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
        mImageWidth = im.cols;
        mImageHeight = im.rows;
        size_t maxTreeSize = TREE_TYPE::getMaxDimensionLength();

        for(size_t y = 0; y < mImageHeight; y+=maxTreeSize){
            for(size_t x = 0; x < mImageWidth; x+=maxTreeSize){
                // use the max size of the tree for the width and hight, or what remains of the image
                size_t width = std::min(maxTreeSize ,  mImageWidth - x);
                size_t height = std::min(maxTreeSize , mImageHeight - y);

                // subregion of the image represented using a single tree
                cv::Rect subRegion(x, y, width, height);
                std::shared_ptr<TREE_TYPE> subtree = std::make_shared<TREE_TYPE>(mProtoTypeTree);
                cv::Mat ROI = im(subRegion);
                subtree->parseImage(ROI);
                addTree(x, y, subtree);
            }
        }
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
#endif //DEPTHCODEC_MULTIPLEQUADTREE_H

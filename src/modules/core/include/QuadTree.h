//
// Created by philip on 7/26/18.
//

#ifndef CAPTURE_TRIANGULATION_QUADTREE_H
#define CAPTURE_TRIANGULATION_QUADTREE_H


#include "NodeAddress.h"
#include <map>
#include <bitset>

template<typename ADDRESS_TYPE, typename LEAF_DATA_TYPE, typename SPLIT_POLICY_TYPE>
class QuadTree : private SPLIT_POLICY_TYPE {
    //size_t mWidth;
    //size_t mHeight;
    size_t mImageWidth;
    size_t mImageHeight;

    typedef typename std::map<ADDRESS_TYPE, LEAF_DATA_TYPE>::iterator Node;

    void prune(const LEAF_DATA_TYPE &parentData, Node tl, Node tr, Node bl, Node br) {
        auto pa = parentAddress(tl->first);
        tree.erase(tl);
        tree.erase(tr);
        tree.erase(bl);
        tree.erase(br);
        addLeaf(pa, parentData);
    }

    using SPLIT_POLICY_TYPE::shouldPrune;

protected:
    std::map<ADDRESS_TYPE, LEAF_DATA_TYPE> tree;

public:
    QuadTree(size_t width, size_t height , const SPLIT_POLICY_TYPE& split):
            SPLIT_POLICY_TYPE(split),
            //mWidth(upperPowerOfTwo(width)),
            //mHeight(upperPowerOfTwo(height)),
            mImageWidth(width),
            mImageHeight(height)
    {}

    size_t getSizeInBtyes(){
        return tree.size() * sizeof(LEAF_DATA_TYPE);
    }

    ADDRESS_TYPE encodeAddress(size_t x, size_t y, size_t l) const{
        return encodeAddressRecurse<NodeAddress32bit>(x,y, maxDimensionLength<ADDRESS_TYPE>()/2, maxDimensionLength<ADDRESS_TYPE>()/2, l);
    }

    std::tuple<size_t, size_t, size_t, size_t> decodeAddress(ADDRESS_TYPE a) const{
        return decodeAddressRecurse(a, 0, 0, maxDimensionLength<ADDRESS_TYPE>(), maxDimensionLength<ADDRESS_TYPE>());
    }

    void addLeaf(ADDRESS_TYPE address, const LEAF_DATA_TYPE& leafData){
        //BOOST_ASSERT_MSG(tree.find(address) == tree.end(), "this should be the first time the tree sees this address");
        tree[address] = leafData;
    }

    // check if the child branches of the tree can be pruned, if so prune.
    bool evaluteForPruning(ADDRESS_TYPE address){
        // first checks if there are four siblings to given address.
        ADDRESS_TYPE pa = parentAddress(address);
        Node tl = tree.find(topLeft(pa));
        Node tr = tree.find(topRight(pa));
        Node bl = tree.find(bottomLeft(pa));
        Node br = tree.find(bottomRight(pa));

        //if four siblings exist, create the parent.
        if(tl != tree.end() && tr != tree.end() && bl != tree.end() && br != tree.end()) {
            LEAF_DATA_TYPE potentalParentNode(tl->second, tr->second, bl->second, br->second);
            // evaluate if the parent should be the new leaf node, i.e. prune the children nodes.
            if (shouldPrune(potentalParentNode)) {
                prune(potentalParentNode, tl, tr, bl, br);
                return true;
            }
        }
        return false;
    }

    //int getWidth() const {return mWidth;}
    //int getHeight() const {return mHeight;}

    cv::Mat decode() const {
        cv::Mat grayImage(mImageHeight, mImageWidth, CV_16UC1, cv::Scalar(0));
        for(auto leaf : this->tree) {
            auto decodedLeaf = this->decodeAddress(leaf.first);
            int x = std::get<0>(decodedLeaf);
            int y = std::get<1>(decodedLeaf);
            int width = std::get<2>(decodedLeaf);
            int height = std::get<3>(decodedLeaf);
            // TODO make this part of the LEAF_NODE_TYPE
            cv::Mat cell = grayImage(cv::Rect(x , y, width, height));//.rowRange(y, y + height).colRange(x , x + width);
            cell.setTo(leaf.second.decodedValue());
        }

        return grayImage;
    }
};


#endif //CAPTURE_TRIANGULATION_QUADTREE_H

//
// Created by philip on 7/26/18.
//

#ifndef CAPTURE_TRIANGULATION_ROLLINGQUADTREE_H
#define CAPTURE_TRIANGULATION_ROLLINGQUADTREE_H

#include <boost/serialization/base_object.hpp>
#include "QuadTree.h"
#include "boost/log/trivial.hpp"
#include "IDepthCodec.h"

/*
 *
 *  The rolling quad tree is a memory sensitive pruning of the quad tree. It builds leafs as it scans across the image,
 *  rolling up quads recursively as new pixels are made avalible.
 *
 *
 *
 the index of the current pixel raster scans accross the image,
 when it reaches a quads bottom right coord such ass 33, it checks it and its siblings can be pruned.
 if so, then it also check the recursively up the tree until either a prune cant be made or the root is reached.

 This example, all attempts to prune succeed. As the scan reaches 3 3 the recursive check to prune not only prunes its siblings,
 but aslo its parents siblings and its 4 grandparents.
                            |                             |
       a = 111111           |       a = 000111            |       a = 000000
                            |                             |
 +-----------+-----------+  |  +-----------+-----------+  | +-----------------------+
 |           |           |  |  |           |           |  | |                       |
 |           |           |  |  |           |           |  | |                       |
 |           |           |  |  |           |           |  | |                       |
 |           |           |  |  |           |           |  | |                       |
 |           |           |  |  |           |           |  | |                       |
 +-----------+-----+-----+  |  +-----------+-----------+  | |                       |
 |           |     |     |  |  |           |           |  | |                       |
 |           |     |     |  |  |           |           |  | |                       |
 |           |-----+-----+  |  |           |           |  | |                       |
 |           |     |     |  |  |           |           |  | |                       |
 | 3 0   3 1 | 3 2 | 3 3 |  |  | 3 0   3 1 | 3 2   3 3 |  | | 3 0   3 1   3 2   3 3 |
 +-----------+-----+-----+  |  +-----------+-----------+  | +-----------------------+
                            |                             |
                            |                             |
                            |                 -           |

 */


template<typename ADDRESS_TYPE, typename LEAF_DATA_TYPE, typename SPLIT_POLICY_TYPE>
class RollingQuadTree: public IDepthCodec, public QuadTree<ADDRESS_TYPE, LEAF_DATA_TYPE, SPLIT_POLICY_TYPE>{
protected:
    void addLeafThenPrune(ADDRESS_TYPE a, const LEAF_DATA_TYPE& leafData){
        // add new cell to tree
        this->addLeaf(a, leafData);
        auto pa = parentAddress(a);
        if(a == bottomRight(pa)){
            /// as this is a bottom right quad, check to see if the parent quad could
            /// also be pruned before moving on
            while(a){
                if(!this->evaluteForPruning(a))
                    break;
                pa = parentAddress(a);
                //printAddress(a);
                a = pa;
            }

        }
    };


public:

    RollingQuadTree(){};

    //TODO make address depth agnostic
    //TODO allow LEAF data to represent more then one pixel
    //TODO TEST at compile time the check that the leaf can represent the cell size
    RollingQuadTree(const SPLIT_POLICY_TYPE& split):QuadTree<ADDRESS_TYPE, LEAF_DATA_TYPE, SPLIT_POLICY_TYPE>(split){};

    virtual void compress(const cv::Mat & im)
    {
		this->setImageSize(im.size());
        size_t maxTreeDepth = maxDepth<ADDRESS_TYPE>();
        BOOST_LOG_TRIVIAL(info) << "Max tree depth: "<< maxTreeDepth;
        for (int y = 0; y < im.rows; y++) {
            for (int x = 0; x < im.cols; x++) {
                // TODO offload cell eval to the leaf, allowing parsing to be depth agnostic
                auto a = this->encodeAddress(x, y, maxTreeDepth);
                //auto depth = im.at<ushort>(y, x);
                //addLeafThenPrune(a, LEAF_DATA_TYPE(depth));
                addLeafThenPrune(a, LEAF_DATA_TYPE(im, cv::Rect(x,y,1,1)));
            }
        }
    }

    virtual void decompress(cv::Mat& depthImage){
        for(auto leaf : this->mLeafs) {
            auto decodedLeaf = this->decodeAddress(leaf.first);
            int x = std::get<0>(decodedLeaf);
            int y = std::get<1>(decodedLeaf);
            int width = std::get<2>(decodedLeaf);
            int height = std::get<3>(decodedLeaf);
            // TODO make this part of the LEAF_NODE_TYPE
            cv::Mat cell = depthImage.rowRange(y, y + height).colRange(x , x + width);
            leaf.second.render(cell);
        }
    }

    virtual bool getMaxImageSize(cv::Size& size) const {
        size = cv::Size(maxDimensionLength<ADDRESS_TYPE>(), maxDimensionLength<ADDRESS_TYPE>());
        return true;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // note, version is always the latest when saving
        boost::serialization::void_cast_register<RollingQuadTree<ADDRESS_TYPE, LEAF_DATA_TYPE, SPLIT_POLICY_TYPE>, IDepthCodec>();
        ar & boost::serialization::base_object<IDepthCodec>(*this);
        ar & boost::serialization::base_object<QuadTree<ADDRESS_TYPE, LEAF_DATA_TYPE, SPLIT_POLICY_TYPE>>(*this);
    }

    friend class boost::serialization::access;
};


#endif //CAPTURE_TRIANGULATION_ROLLINGQUADTREE_H


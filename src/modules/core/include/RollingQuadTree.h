//
// Created by philip on 7/26/18.
//

#ifndef CAPTURE_TRIANGULATION_ROLLINGQUADTREE_H
#define CAPTURE_TRIANGULATION_ROLLINGQUADTREE_H


#include "QuadTree.h"


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

template<typename LEAF_DATA_TYPE, typename SPLIT_POLICY_TYPE>
class RollingQuadTree: public QuadTree<LEAF_DATA_TYPE, SPLIT_POLICY_TYPE>{
    int mMaxDepth = 10;


public:
    /// Perfect tree with every prune being possible
    RollingQuadTree(const SPLIT_POLICY_TYPE& split):QuadTree<LEAF_DATA_TYPE, SPLIT_POLICY_TYPE>(1024, 1024, split)
    {
        //mMaxDepth = MAX_DEPTH;
        BOOST_LOG_TRIVIAL(info) << "Max tree depth: "<< mMaxDepth;

        for (int y = 0; y < this->getHeight(); y++) {
            for (int x = 0; x < this->getWidth(); x++) {
                auto a = this->encodeAddress(x, y, 10);
                addLeafThenPrune(a, LEAF_DATA_TYPE(1));
            }
        }
    }

    RollingQuadTree(const cv::Mat & im, const SPLIT_POLICY_TYPE& split):
            QuadTree<LEAF_DATA_TYPE, SPLIT_POLICY_TYPE>(im.cols, im.rows, split)
    {
        //mMaxDepth = MAX_DEPTH;
        BOOST_LOG_TRIVIAL(info) << "Max tree depth: "<< mMaxDepth;
        for (int y = 0; y < im.rows; y++) {
            for (int x = 0; x < im.cols; x++) {
                auto a = this->encodeAddress(x, y, mMaxDepth);
                auto depth = im.at<ushort>(y, x);
                addLeafThenPrune(a, LEAF_DATA_TYPE(depth));
            }
        }
    }

    void addLeafThenPrune(NodeAddress a, const LEAF_DATA_TYPE& leafData){
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

};



#endif //CAPTURE_TRIANGULATION_ROLLINGQUADTREE_H

//
// Created by philip on 7/26/18.
//

#ifndef CAPTURE_TRIANGULATION_QUADTREE_H
#define CAPTURE_TRIANGULATION_QUADTREE_H


#include "NodeAddress.h"
#include <map>
#include <bitset>
#include "Tree.h"
#include <boost/serialization/base_object.hpp>
#include <vector>

template<typename ADDRESS_TYPE, typename LEAF_DATA_TYPE, typename SPLIT_POLICY_TYPE>
class QuadTree : public Tree<ADDRESS_TYPE, LEAF_DATA_TYPE>, private SPLIT_POLICY_TYPE {
   using SPLIT_POLICY_TYPE::shouldPrune;

protected:
    QuadTree(){};
public:

    typedef Tree<ADDRESS_TYPE, LEAF_DATA_TYPE> TREE_TYPE;

    QuadTree(const SPLIT_POLICY_TYPE& split):
            SPLIT_POLICY_TYPE(split)
    {}

	ADDRESS_TYPE encodeAddress(size_t x, size_t y, size_t l) const{
        return encodeAddressRecurse<NodeAddress32bit>(x,y, maxDimensionLength<ADDRESS_TYPE>()/2, maxDimensionLength<ADDRESS_TYPE>()/2, l);
    }

    std::tuple<size_t, size_t, size_t, size_t> decodeAddress(ADDRESS_TYPE a) const{
        return decodeAddressRecurse(a, 0, 0, maxDimensionLength<ADDRESS_TYPE>(), maxDimensionLength<ADDRESS_TYPE>());
    }

    // check if the child branches of the tree can be pruned, if so prune.
    bool evaluteForPruning(ADDRESS_TYPE address){
        // first checks if there are four siblings to given address.
        ADDRESS_TYPE pa = parentAddress(address);
		auto tl = this->getLeaf(topLeft(pa));
		auto tr = this->getLeaf(topRight(pa));
		auto bl = this->getLeaf(bottomLeft(pa));
		auto br = this->getLeaf(bottomRight(pa));

        //if four siblings exist, can create the parent.
        if(tl != this->mLeafs.end() && tr != this->mLeafs.end() && bl != this->mLeafs.end() && br != this->mLeafs.end()) {
			LEAF_DATA_TYPE potentalParentNode({tl->second, tr->second, bl->second, br->second });
            // evaluate if the parent should be the new leaf node, i.e. prune the children nodes.
            if (shouldPrune(potentalParentNode)) 
			{
                std::vector<typename TREE_TYPE::LeafNode> children;
                children.reserve(4);
                children.push_back(tl);
                children.push_back(tr);
                children.push_back(bl);
                children.push_back(br);
				this->prune(potentalParentNode, children);
                return true;
            }
        }
        return false;
    }

    static size_t getMaxDimensionLength(){
        return maxDimensionLength<ADDRESS_TYPE>();
    }


    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // note, version is always the latest when saving
        ar & boost::serialization::base_object<TREE_TYPE>(*this);
    }

    friend class boost::serialization::access;
};


#endif //CAPTURE_TRIANGULATION_QUADTREE_H

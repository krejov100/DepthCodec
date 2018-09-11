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

template<typename ADDRESS_TYPE, typename LEAF_DATA_TYPE, typename SPLIT_POLICY_TYPE>
class QuadTree : private SPLIT_POLICY_TYPE {
   using SPLIT_POLICY_TYPE::shouldPrune;
   using TREE_TYPE = Tree<typename ADDRESS_TYPE, typename LEAF_DATA_TYPE>;
protected:
	TREE_TYPE mTree;
    QuadTree(){};
public:
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
		auto tl = mTree.getLeaf(topLeft(pa));
		auto tr = mTree.getLeaf(topRight(pa));
		auto bl = mTree.getLeaf(bottomLeft(pa));
		auto br = mTree.getLeaf(bottomRight(pa));

        //if four siblings exist, can create the parent.
        if(tl != mTree.end() && tr != mTree.end() && bl != mTree.end() && br != mTree.end()) {
			LEAF_DATA_TYPE potentalParentNode({tl->second, tr->second, bl->second, br->second });
            // evaluate if the parent should be the new leaf node, i.e. prune the children nodes.
            if (shouldPrune(potentalParentNode)) 
			{
				mTree.prune(potentalParentNode, std::vector<TREE_TYPE::LeafNode> { tl, tr, bl, br });
                return true;
            }
        }
        return false;
    }

    static size_t getMaxDimensionLength(){
        return maxDimensionLength<ADDRESS_TYPE>();
    }


    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) const
    {
        // note, version is always the latest when saving
        ar & tree;
    }

    friend class boost::serialization::access;
};


#endif //CAPTURE_TRIANGULATION_QUADTREE_H

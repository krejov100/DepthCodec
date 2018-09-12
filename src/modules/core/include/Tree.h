#include <map>
#include <boost/serialization/map.hpp>

// a basic tree that uses a map internally
template<typename ADDRESS_TYPE, typename LEAF_DATA_TYPE>
class Tree
{
protected:
	std::map<ADDRESS_TYPE, LEAF_DATA_TYPE> mLeafs;
public:

	typedef typename std::map<ADDRESS_TYPE, LEAF_DATA_TYPE>::iterator LeafNode;

	size_t getSizeInBtyes()
	{
		return mLeafs.size() * sizeof(LEAF_DATA_TYPE);
	}

	void addLeaf(ADDRESS_TYPE address, const LEAF_DATA_TYPE& leafData)
	{
		//BOOST_ASSERT_MSG(tree.find(address) == tree.end(), "this should be the first time the tree sees this address");
		mLeafs[address] = leafData;
	}

	LeafNode getLeaf(ADDRESS_TYPE address) {
		return mLeafs.find(address);
	}

	LeafNode& begin() {
		return mLeafs.begin();
	}

	LeafNode& end()
	{
		return mLeafs.end();
	}
	
	void prune(const LEAF_DATA_TYPE &parentData, std::vector<LeafNode>& children)
	{
		auto pa = parentAddress(children[0]->first);
		for (auto &child : children)
		{
			BOOST_ASSERT(parentAddress(child->first) == pa);
			mLeafs.erase(child);
		}
		addLeaf(pa, parentData);
	}
	
	size_t getLeafCount() 
	{
		return mLeafs.size();
	}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		// note, version is always the latest when saving
		ar & mLeafs;
	}

	friend class boost::serialization::access;
};
#include <map>

// a basic tree that uses a map internally
template<typename ADDRESS_TYPE, typename LEAF_DATA_TYPE>
class Tree
{
	std::map<ADDRESS_TYPE, LEAF_DATA_TYPE> mLeafs;
public:
	size_t getSizeInBtyes()
	{
		return mLeafs.size() * sizeof(LEAF_DATA_TYPE);
	}

	void addLeaf(ADDRESS_TYPE address, const LEAF_DATA_TYPE& leafData)
	{
		//BOOST_ASSERT_MSG(tree.find(address) == tree.end(), "this should be the first time the tree sees this address");
		mLeafs[address] = leafData;
	}

	typedef typename std::map<ADDRESS_TYPE, LEAF_DATA_TYPE>::iterator LeafNode;	
	typedef typename const std::map<ADDRESS_TYPE, LEAF_DATA_TYPE>::iterator const_LeafNode;

	LeafNode& getLeaf(ADDRESS_TYPE address) {
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

	friend std::ostream &operator<<(std::ostream &, const Tree<ADDRESS_TYPE, LEAF_DATA_TYPE> &);
	friend std::istream &operator>>(std::istream &, Tree<ADDRESS_TYPE, LEAF_DATA_TYPE> &);
};

template<typename ADDRESS_TYPE, typename LEAF_DATA_TYPE>
std::ostream operator<<(std::ostream &os, const Tree<ADDRESS_TYPE, LEAF_DATA_TYPE> &v)
{
	os << getLeafCount();
	std::copy(v.begin(), v.end(), std::ostream_iterator<T>(std::cout));
	return os;
}

template<typename ADDRESS_TYPE, typename LEAF_DATA_TYPE>
std::istream operator>>(std::istream &is, Tree<ADDRESS_TYPE, LEAF_DATA_TYPE> &v)
{
	size_t leafCount;
	os >> leafCount;
	std::copy(std::istream_iterator)
	mLeafs
	return is;
}
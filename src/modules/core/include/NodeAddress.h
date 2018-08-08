//
// Created by philip on 7/26/18.
//

#ifndef CAPTURE_TRIANGULATION_NODEADDRESS_H
#define CAPTURE_TRIANGULATION_NODEADDRESS_H


using NodeAddress = unsigned int;
const size_t MAX_DEPTH = (sizeof(NodeAddress) * 8) /3;


#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) ((a) & (1ULL<<(b)))

///https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
unsigned int upperPowerOfTwo(unsigned int v)
{
    v--;
    v |= v >> 1u;
    v |= v >> 2u;
    v |= v >> 4u;
    v |= v >> 8u;
    v |= v >> 16u;
    v++;
    return v;
}

/// cx, cy are middle of the cell. l is the depth, d is the current recursion depth;
constexpr NodeAddress encodeAddressRecurse(size_t x, size_t y, size_t cx, size_t cy, size_t l, size_t d =0, NodeAddress a = NodeAddress()){
    //std::cout << x << " " << y << " " << cx << " " << cy << " " << l << " " << d << " ";
    BIT_SET(a, (d * 3)+2);
    if(x >= cx) {
        BIT_SET(a, (d * 3));
        x -= cx;
    }
    if(y >= cy) {
        BIT_SET(a, (d * 3)+1);
        y -= cy;
    }
    //printAddress(a);
    //std::cout << std::endl;
    if(d < l-1)
        a = encodeAddressRecurse(x, y, cx/2, cy/2, l, ++d, a);
    return a;
}

constexpr std::tuple<size_t, size_t, size_t, size_t> decodeAddressRecurse(NodeAddress a, size_t x, size_t y, size_t dx, size_t dy){
    if(a == 0)
        return std::make_tuple(x, y, dx, dy);
    else
    {
        x += (a & 0b01 ? dx/2 : 0);
        y += (a & 0b10 ? dy/2 : 0);
        return decodeAddressRecurse(a >> 3, x, y, dx/2, dy/2);
    }
};


#ifdef WITH_CLZ
constexpr size_t getDepthFastImpl(NodeAddress address){
    return  (((sizeof(NodeAddress)*8) -__builtin_clz(address)) / 3);
}
#endif // WITH_CLZ

inline size_t getDepthSlowImpl(NodeAddress address){
    size_t length = sizeof(NodeAddress)*8;
    size_t mostSig = 0;
    for(size_t i = 0; i < length; i++){
        auto bits = std::bitset<32>(address);
        if(bits.test(i))
            mostSig = i;
    }
    return (mostSig/3) + 1;
}

inline size_t getDepth(NodeAddress address){
#ifdef WITH_CLZ
    return getDepthFastImpl(address);
#else
    return getDepthSlowImpl(address);
#endif
}


inline void printAddress(const NodeAddress address){
    std::cout<<"Address: " << address << " " << std::bitset<32>(address) << std::endl;
}


inline NodeAddress parentAddress(const NodeAddress& address){
    size_t shift = (getDepth(address) - 1)*3;
    return address & ~(0b111 << shift);
}
inline NodeAddress topLeft(NodeAddress parentAddress){
    size_t shift = getDepth(parentAddress)*3;
    return parentAddress | (0b100 << shift);
}
inline NodeAddress topRight(NodeAddress parentAddress){
    size_t shift = getDepth(parentAddress)*3;
    return parentAddress | (0b101 << shift);
}
inline NodeAddress bottomLeft(NodeAddress parentAddress){
    size_t shift = getDepth(parentAddress)*3;
    return parentAddress | (0b110 << shift);
}
inline NodeAddress bottomRight(NodeAddress parentAddress){
    size_t shift = getDepth(parentAddress)*3;
    return  parentAddress | (0b111 << shift);
}


#endif //CAPTURE_TRIANGULATION_NODEADDRESS_H

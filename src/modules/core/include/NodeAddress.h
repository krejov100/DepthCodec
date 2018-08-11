//
// Created by philip on 7/26/18.
//

#ifndef CAPTURE_TRIANGULATION_NODEADDRESS_H
#define CAPTURE_TRIANGULATION_NODEADDRESS_H

#include <iostream>
#include <tuple>
#include <bitset>
#include <math.h>

using NodeAddress32bit = unsigned int;
using NodeAddress16bit = unsigned short;

template<typename ADDRESS_TYPE>
constexpr size_t maxDepth() {
    return (sizeof(ADDRESS_TYPE) * 8) / 3;
}

template<typename ADDRESS_TYPE>
constexpr size_t maxDimensionLength(){
    return exp2(maxDepth<ADDRESS_TYPE>());
}

#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) ((a) & (1ULL<<(b)))

///https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
constexpr unsigned int upperPowerOfTwo(unsigned int v)
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
template<typename ADDRESS_TYPE>
constexpr ADDRESS_TYPE encodeAddressRecurse(size_t x, size_t y, size_t cx, size_t cy, size_t l, size_t d =0, ADDRESS_TYPE a = ADDRESS_TYPE()){
    BIT_SET(a, (d * 3)+2);
    if(x >= cx) {
        BIT_SET(a, (d * 3));
        x -= cx;
    }
    if(y >= cy) {
        BIT_SET(a, (d * 3)+1);
        y -= cy;
    }
    if(d < l-1)
        a = encodeAddressRecurse(x, y, cx/2, cy/2, l, ++d, a);
    return a;
}

template<typename ADDRESS_TYPE>
constexpr std::tuple<size_t, size_t, size_t, size_t> decodeAddressRecurse(ADDRESS_TYPE a, size_t x, size_t y, size_t dx, size_t dy){
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
template<typename ADDRESS_TYPE>
constexpr size_t getDepthFastImpl(ADDRESS_TYPE address){
    return  (((sizeof(ADDRESS_TYPE)*8) -__builtin_clz(address)) / 3);
}
#endif // WITH_CLZ

template<typename ADDRESS_TYPE>
constexpr size_t getDepthSlowImpl(ADDRESS_TYPE address){
    size_t length = sizeof(ADDRESS_TYPE)*8;
    size_t mostSig = 0;
    for(size_t i = 0; i < length; i++){
        auto bits = std::bitset<32>(address);
        if(bits.test(i))
            mostSig = i;
    }
    return (mostSig/3) + 1;
}

template<typename ADDRESS_TYPE>
constexpr size_t getDepth(ADDRESS_TYPE address){
#ifdef WITH_CLZ
    return getDepthFastImpl(address);
#else
    return getDepthSlowImpl(address);
#endif
}

template<typename ADDRESS_TYPE>
inline void printAddress(const ADDRESS_TYPE address){
    std::cout<<"Address: " << address << " " << std::bitset<32>(address) << std::endl;
}

template<typename ADDRESS_TYPE>
constexpr ADDRESS_TYPE parentAddress(const ADDRESS_TYPE& address){
    size_t shift = (getDepth(address) - 1)*3;
    return address & ~(0b111 << shift);
}

template<typename ADDRESS_TYPE>
constexpr ADDRESS_TYPE topLeft(ADDRESS_TYPE parentAddress){
    size_t shift = getDepth(parentAddress)*3;
    return parentAddress | (0b100 << shift);
}

template<typename ADDRESS_TYPE>
constexpr ADDRESS_TYPE topRight(ADDRESS_TYPE parentAddress){
    size_t shift = getDepth(parentAddress)*3;
    return parentAddress | (0b101 << shift);
}

template<typename ADDRESS_TYPE>
constexpr ADDRESS_TYPE bottomLeft(ADDRESS_TYPE parentAddress){
    size_t shift = getDepth(parentAddress)*3;
    return parentAddress | (0b110 << shift);
}

template<typename ADDRESS_TYPE>
constexpr ADDRESS_TYPE bottomRight(ADDRESS_TYPE parentAddress){
    size_t shift = getDepth(parentAddress)*3;
    return  parentAddress | (0b111 << shift);
}

#endif //CAPTURE_TRIANGULATION_NODEADDRESS_H

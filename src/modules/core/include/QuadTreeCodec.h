//
// Created by philip on 8/9/18.
//

#ifndef DEPTHCODEC_QUADTREECODEC_H
#define DEPTHCODEC_QUADTREECODEC_H

#include "IDepthCodec.h"

/// if the quadtree addressing is too shallow to represent the entire width and height of the image,
/// use a ROI representation for the leafs, example:
/// A 1920 x 1080 image is too width for every pixel to be represented using a quadtree address of 32bit.
/// This is because a 32bit address can represent a max depth of floor(32/3) = 10 as 3bits per depth,
/// and 2^10 is 1024

/// we can solve this in two ways, represent the image with multiple trees,
/// adding an x and y to specify which image region they specify. eg 4 trees can represent a 1920 x 1080 image.
/// alternativly, we can represent the smallest part of the image with patches instead of pixels.

/// Really we should consider a trade off of both approches.

class QuadTreeCodec : public IDepthCodec{
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
public:


};

#endif //DEPTHCODEC_QUADTREECODEC_H

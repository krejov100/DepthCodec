//
// Created by philip on 9/13/18.
//

#ifndef DEPTHCODEC_DEPTHCODEC_H
#define DEPTHCODEC_DEPTHCODEC_H

#include "IDepthCodec.h"
#include "QuadTreeTypes.h"

class DepthCodec{
public:
    IDepthCodec* pimpl;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int file_version){
        ar & pimpl;
    }
};

#endif //DEPTHCODEC_DEPTHCODEC_H


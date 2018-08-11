//
// Created by philip on 8/9/18.
//


#include "QuadTreeCodecFactory.h"
#include "QuadTreeTypes.h"

std::shared_ptr<IDepthCodec> QuadTreeCodecFactory::construct(const po::variables_map& options){
    RollingQT32bitMinMaxAbsDiff prototypeTree(AbsDiffPolicy(30 * (65536/255)));
    return std::move(std::make_unique<MultipleQuadTreeCodec<RollingQT32bitMinMaxAbsDiff>>(prototypeTree));
}

po::options_description QuadTreeCodecFactory::getOptions(){
    po::options_description desc("QuadTree Compression");
    desc.add_options()
            ("TreeType", po::value<std::string>(),
             "address size used to address nodes in the tree")
            ;

    return desc;
}
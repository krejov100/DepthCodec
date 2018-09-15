//
// Created by philip on 8/9/18.
//


#include "QuadTreeCodecFactory.h"
#include "QuadTreeTypes.h"

QuadTreeCodecFactory::QuadTreeCodecFactory(const po::variables_map& options):DepthCodecFactory(options){};

std::shared_ptr<IDepthCodec> QuadTreeCodecFactory::construct(){
    return std::make_shared<RollingQT32bitMinMaxAbsDiff>(AbsDiffPolicy(30 * (65536/255)));
}

po::options_description QuadTreeCodecFactory::getOptions(){
    po::options_description desc("QuadTree Compression");
    desc.add_options()
            ("TreeType", po::value<std::string>(),
             "address size used to address nodes in the tree")
            ;

    return desc;
}
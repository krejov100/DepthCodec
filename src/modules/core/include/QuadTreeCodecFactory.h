//
// Created by philip on 8/9/18.
//

#ifndef DEPTHCODEC_QUADTREECODECFACTORY_H
#define DEPTHCODEC_QUADTREECODECFACTORY_H

#include "Factory.h"
#include "DepthCodecFactory.h"

namespace po = boost::program_options;

/// This class is used to configure a compression method, popluating the pimpl
class QuadTreeCodecFactory: public DepthCodecFactory{
public:
    QuadTreeCodecFactory(const po::variables_map& options):DepthCodecFactory(options){};
    std::shared_ptr<IDepthCodec> construct();
    static po::options_description getOptions();
};

#endif //DEPTHCODEC_QUADTREECODECFACTORY_H

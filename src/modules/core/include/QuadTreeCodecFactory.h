//
// Created by philip on 8/9/18.
//

#ifndef DEPTHCODEC_QUADTREECODECFACTORY_H
#define DEPTHCODEC_QUADTREECODECFACTORY_H

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include "IDepthCodec.h"

namespace po = boost::program_options;

/// This class is used to configure a compression method, popluating the pimpl
class QuadTreeCodecFactory{
public:
    static std::shared_ptr<IDepthCodec> construct(const po::variables_map& options);
    static po::options_description getOptions();
};

#endif //DEPTHCODEC_QUADTREECODECFACTORY_H

//
// Created by philip on 8/9/18.
//

#ifndef DEPTHCODEC_DEPTHCOMPRESSIONFACTORY_H
#define DEPTHCODEC_DEPTHCOMPRESSIONFACTORY_H

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include "IDepthCodec.h"

namespace po = boost::program_options;

/// This class is used to configure a compression method, popluating the pimpl
class DepthCodecFactory{
    /// string to sub factory map
    static std::map<std::string, std::function<std::shared_ptr<IDepthCodec>(const po::variables_map&)>> subCodecFactory;
public:
    DepthCodecFactory();
    static std::shared_ptr<IDepthCodec> construct(const po::variables_map& options);
    static po::options_description getOptions();
};

#endif //DEPTHCODEC_DEPTHCOMPRESSIONFACTORY_H

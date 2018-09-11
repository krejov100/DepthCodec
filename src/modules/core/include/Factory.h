//
// Created by philip on 8/24/18.
//

#ifndef DEPTHCODEC_FACTORY_H
#define DEPTHCODEC_FACTORY_H

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>

namespace po = boost::program_options;

class Factory{
protected:
    po::variables_map mOptions;
public:
    Factory(const po::variables_map& options): mOptions(options){};
    virtual void reconfigure(const po::variables_map& options){mOptions = options;};
};

#endif //DEPTHCODEC_FACTORY_H

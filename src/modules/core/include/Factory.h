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
    /// string to sub factory map
    std::map<std::string, std::shared_ptr<Factory>> mSubCodecFactories;
public:
    Factory(){};
    Factory(const po::variables_map& options): mOptions(options){};

    virtual void reconfigure(const po::variables_map& options)
    {
        mOptions = options;
    };

    virtual void registerSubFactory(std::string name, std::shared_ptr<Factory> childFactory)
    {
        mSubCodecFactories.insert(std::make_pair(name, childFactory));
    };

    virtual po::options_description getOptions() = 0;
};

#endif //DEPTHCODEC_FACTORY_H

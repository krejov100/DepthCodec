//
// Created by philip on 8/9/18.
//

#pragma once
#include "IDepthCodec.h"
#include "Factory.h"

/// This class is used to configure a compression method, popluating the pimpl
/// it is a delegating factory th
class DepthCodecFactory: public Factory{
    /// string to sub factory map
    std::map<std::string, std::shared_ptr<DepthCodecFactory>> mSubCodecFactories;

public:
    DepthCodecFactory(const po::variables_map& options);
    void registerSubFactory(std::string name, std::shared_ptr<DepthCodecFactory> childFactory);
    virtual std::shared_ptr<IDepthCodec> construct();
    static po::options_description getOptions();
};

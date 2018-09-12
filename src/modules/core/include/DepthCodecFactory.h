//
// Created by philip on 8/9/18.
//

#pragma once
#include "IDepthCodec.h"
#include "Factory.h"

/// This class is used to configure a compression method, popluating the pimpl
/// it is a delegating factory th
class DepthCodecFactory: public Factory{

public:
    DepthCodecFactory(){};
    DepthCodecFactory(const po::variables_map& options);
    virtual po::options_description getOptions();
    virtual std::shared_ptr<IDepthCodec> construct();
};

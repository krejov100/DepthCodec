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
    std::map<std::string, std::function<std::shared_ptr<IDepthCodec>()>> mSubCodecFactories;
    void registerSubFactory(std::string name, DepthCodecFactory* childFactory);

public:
	DepthCodecFactory(const po::variables_map& options);
	
	virtual std::shared_ptr<IDepthCodec> construct();
    static po::options_description getOptions();
};

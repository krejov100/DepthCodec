//testFramework
// Created by philip on 8/9/18.
//

#include <memory>
#include <DepthCodecFactory.h>
#include "IDepthCodec.h"
#include "NodeAddress.h"
#include <map>
#include <QuadTreeCodecFactory.h>

DepthCodecFactory::DepthCodecFactory(const po::variables_map& options):Factory(options){}

std::shared_ptr<IDepthCodec> DepthCodecFactory::construct(){
    std::string factoryType = mOptions["CodecType"].as<std::string>();
    auto subFactory = mSubCodecFactories.find(factoryType);
    BOOST_ASSERT_MSG(subFactory != mSubCodecFactories.end(), "Invalid Codec Type or Child Factory not registered");
    return std::static_pointer_cast<DepthCodecFactory>(subFactory->second)->construct();
}

po::options_description DepthCodecFactory::getOptions(){

    po::options_description desc("Compression Configuration");
    desc.add_options()
            ("CodecType", po::value<std::string>())
            ;
    return desc;
}



//testFramework
// Created by philip on 8/9/18.
//
#include "DepthCodecFactory.h"
#include <map>
#include "IDepthCodec.h"
#include "NodeAddress.h"
#include "QuadTreeCodecFactory.h"

DepthCodecFactory::DepthCodecFactory(const po::variables_map& options):Factory(options) {}

void DepthCodecFactory::registerSubFactory(std::string name, DepthCodecFactory* childFactory)
{
    mSubCodecFactories.insert(std::make_pair(name, std::bind(&DepthCodecFactory::construct,childFactory)));
};

std::shared_ptr<IDepthCodec> DepthCodecFactory::construct(){
    return mSubCodecFactories.find(mOptions["CodecType"].as<std::string>())->second();
}

po::options_description DepthCodecFactory::getOptions(){

    po::options_description desc("Compression Configuration");
    desc.add_options()
            ("CodecType", po::value<std::string>()->default_value("QuadTree"))
            ;
    desc.add(QuadTreeCodecFactory::getOptions());

    return desc;
}



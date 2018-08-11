//testFramework
// Created by philip on 8/9/18.
//

#include <DepthCodecFactory.h>
#include "IDepthCodec.h"
#include "NodeAddress.h"
#include <map>
#include <QuadTreeCodecFactory.h>



std::shared_ptr<IDepthCodec> DepthCodecFactory::construct(const po::variables_map& options){
    return subCodecFactory.find(options["CodecType"].as<std::string>())->second(options);
}

po::options_description DepthCodecFactory::getOptions(){

    po::options_description desc("Compression Configuration");
    desc.add_options()
            ("CodecType", po::value<std::string>()->default_value("QuadTree"))
            ;
    desc.add(QuadTreeCodecFactory::getOptions());

    return desc;
}

std::map<std::string, std::function<std::shared_ptr<IDepthCodec>(const po::variables_map&)>> DepthCodecFactory::subCodecFactory = {{"QuadTree", &QuadTreeCodecFactory::construct}};
#pragma once

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include "Marshaller.hpp"

/// Boost marshaller that converts to txt
template<typename MESSAGE_TYPE>
class BoostMarshaller:public Marshaller<MESSAGE_TYPE>{
public:
    /// default constructor
    BoostMarshaller() = default;
    ~BoostMarshaller(){}

    /// marshall a message into byte data for transport
    DataStream marshall(const MESSAGE_TYPE& message) const final{

        DataStream ds;
        boost::iostreams::back_insert_device<DataStream> inserter(ds);
        boost::iostreams::stream<boost::iostreams::back_insert_device<DataStream> > s(inserter);
        boost::archive::binary_oarchive oa(s);
        oa << message;
        s.flush();

        return ds;
    };

    /// unmarshall a message from bytes
    MESSAGE_TYPE unmarshall(const DataStream& dataStream) const final{
        //std::string str(dataStream.begin(), dataStream.end());
        //std::istringstream iss(str);

        boost::iostreams::basic_array_source<char> data(dataStream.data(), dataStream.size());
        boost::iostreams::stream<boost::iostreams::basic_array_source<char>> stream(data);

        boost::archive::binary_iarchive ia(stream);
        MESSAGE_TYPE msg;
        ia >> msg;
        return msg;
    };
};
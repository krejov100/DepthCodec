#pragma once

#include <string>
#include <cstring>
#include "DataStream.hpp"

/// Prepares a message for transport, possibly through serialisation
template<typename MESSAGE_TYPE>
class Marshaller{
public:
    /// default constructor
    Marshaller() = default;

    /// marshall a message into byte data for transport
    virtual DataStream marshall(const MESSAGE_TYPE& message) const = 0;

    /// unmarshall a message from bytes
    virtual MESSAGE_TYPE unmarshall(const DataStream& byteStream) const = 0;
};

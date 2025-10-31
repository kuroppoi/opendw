#ifndef __MESSAGE_PACK_PARSER_H__
#define __MESSAGE_PACK_PARSER_H__

#include "axmol.h"

namespace opendw::msgpack
{

class ParseException : std::runtime_error
{
public:
    explicit ParseException(const std::string& message) : std::runtime_error(message) {}
    explicit ParseException(const char* message) : std::runtime_error(message) {}
};

class MessagePackParser
{
public:
    MessagePackParser(const uint8_t* input, size_t length) : _input(input), _length(length), _position(0) {}

    void unpackNil();

    bool unpackBool();

    float unpackFloat();
    double unpackDouble();

    uint8_t unpackUInt8();
    uint16_t unpackUInt16();
    uint32_t unpackUInt32();
    uint64_t unpackUInt64();

    int8_t unpackInt8();
    int16_t unpackInt16();
    int32_t unpackInt32();
    int64_t unpackInt64();

    std::string unpackString();

    uint32_t unpackMapStart();
    ax::ValueMap unpackMap();

    uint32_t unpackArrayStart();
    ax::ValueVector unpackArray();

    ax::Value unpackValue();

private:
    uint8_t readUInt8();
    uint16_t readUInt16();
    uint32_t readUInt32();
    uint64_t readUInt64();

    int8_t readInt8(bool checkRange = false);
    int16_t readInt16(bool checkRange = false);
    int32_t readInt32(bool checkRange = false);
    int64_t readInt64(bool checkRange = false);

    void ensureEnoughBytes(size_t length) const;
    void throwInvalidType(const std::string& expected, uint8_t receivedToken) const;

    const uint8_t* _input;
    size_t _length;
    size_t _position;
};

}  // namespace opendw::msgpack

#endif  // __MESSAGE_PACK_PARSER_H__

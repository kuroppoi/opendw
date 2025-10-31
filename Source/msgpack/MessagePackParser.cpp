#include "MessagePackParser.h"

#include "msgpack/MessagePack.h"

#define POS_FIXINT_MASK      0b10000000
#define NEG_FIXINT_MASK      0b11100000
#define FIXSTRING_MASK       0b11100000
#define FIXSTRING_BITS       0b10100000
#define FIXSTRING_LEN_BITS   0b00011111
#define FIXMAP_MASK          0b11110000
#define FIXMAP_BITS          0b10000000
#define FIXMAP_LEN_BITS      0b00001111
#define FIXARRAY_MASK        0b11110000
#define FIXARRAY_BITS        0b10010000
#define FIXARRAY_LEN_BITS    0b00001111
#define IS_POS_FIXINT(token) (token & POS_FIXINT_MASK) == 0
#define IS_NEG_FIXINT(token) (token & NEG_FIXINT_MASK) == NEG_FIXINT_MASK
#define IS_FIXINT(token)     IS_POS_FIXINT(token) || IS_NEG_FIXINT(token)
#define IS_FIXSTRING(token)  (token & FIXSTRING_MASK) == FIXSTRING_BITS
#define IS_FIXMAP(token)     (token & FIXMAP_MASK) == FIXMAP_BITS
#define IS_FIXARRAY(token)   (token & FIXARRAY_MASK) == FIXARRAY_BITS

USING_NS_AX;

namespace opendw::msgpack
{

void MessagePackParser::unpackNil()
{
    auto token = readUInt8();
    auto type  = static_cast<DataType>(token);

    if (type != DataType::NIL)
    {
        throwInvalidType("NIL", token);
    }
}

bool MessagePackParser::unpackBool()
{
    auto token = readUInt8();
    auto type  = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::BOOL_FALSE:
        return false;
    case DataType::BOOL_TRUE:
        return true;
    default:
        throwInvalidType("BOOL", token);
    }
}

float MessagePackParser::unpackFloat()
{
    auto token = readUInt8();
    auto type  = static_cast<DataType>(token);

    if (type != DataType::FLOAT_32)
    {
        throwInvalidType("FLOAT_32", token);
    }

    // TODO: might not work on all architectures
    return std::bit_cast<float>(readUInt32());
}

double MessagePackParser::unpackDouble()
{
    auto token = readUInt8();
    auto type  = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::FLOAT_64:
        // TODO: might not work on all architectures
        return std::bit_cast<double>(readUInt64());
    case DataType::FLOAT_32:
        // TODO: might not work on all architectures
        return std::bit_cast<float>(readUInt32());
    default:
        throwInvalidType("FLOAT_64", token);
    }
}

uint8_t MessagePackParser::unpackUInt8()
{
    auto token = readUInt8();

    if (IS_POS_FIXINT(token))
    {
        return token;
    }
    else if (IS_NEG_FIXINT(token))
    {
        throwInvalidType("UINT_8", token);
    }

    auto type = static_cast<DataType>(token);

    if (type != DataType::UINT_8)
    {
        throwInvalidType("UINT_8", token);
    }

    return readUInt8();
}

uint16_t MessagePackParser::unpackUInt16()
{
    auto token = readUInt8();

    if (IS_POS_FIXINT(token))
    {
        return token;
    }
    else if (IS_NEG_FIXINT(token))
    {
        throwInvalidType("UINT_16", token);
    }

    auto type = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::UINT_16:
        return readUInt16();
    case DataType::UINT_8:
        return readUInt8();
    default:
        throwInvalidType("UINT_16", token);
    }
}

uint32_t MessagePackParser::unpackUInt32()
{
    auto token = readUInt8();

    if (IS_POS_FIXINT(token))
    {
        return token;
    }
    else if (IS_NEG_FIXINT(token))
    {
        throwInvalidType("UINT_32", token);
    }

    auto type = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::UINT_32:
        return readUInt32();
    case DataType::UINT_16:
        return readUInt16();
    case DataType::UINT_8:
        return readUInt8();
    default:
        throwInvalidType("UINT_32", token);
    }
}

uint64_t MessagePackParser::unpackUInt64()
{
    auto token = readUInt8();

    if (IS_POS_FIXINT(token))
    {
        return token;
    }
    else if (IS_NEG_FIXINT(token))
    {
        throwInvalidType("UINT_64", token);
    }

    auto type = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::UINT_64:
        return readUInt64();
    case DataType::UINT_32:
        return readUInt32();
    case DataType::UINT_16:
        return readUInt16();
    case DataType::UINT_8:
        return readUInt8();
    default:
        throwInvalidType("UINT_64", token);
    }
}

int8_t MessagePackParser::unpackInt8()
{
    auto token = readUInt8();

    if (IS_FIXINT(token))
    {
        return static_cast<int8_t>(token);
    }

    auto type = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::UINT_8:
        return readInt8(true);
    case DataType::INT_8:
        return readInt8();
    default:
        throwInvalidType("INT_8", token);
    }
}

int16_t MessagePackParser::unpackInt16()
{
    auto token = readUInt8();

    if (IS_FIXINT(token))
    {
        return token;
    }

    auto type = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::UINT_16:
        return readInt16(true);
    case DataType::INT_16:
        return readInt16();
    case DataType::UINT_8:
        return readInt8(true);
    case DataType::INT_8:
        return readInt8();
    default:
        throwInvalidType("INT_16", token);
    }
}

int32_t MessagePackParser::unpackInt32()
{
    auto token = readUInt8();

    if (IS_FIXINT(token))
    {
        return token;
    }

    auto type = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::UINT_32:
        return readInt32(true);
    case DataType::INT_32:
        return readInt32();
    case DataType::UINT_16:
        return readInt16(true);
    case DataType::INT_16:
        return readInt16();
    case DataType::UINT_8:
        return readInt8(true);
    case DataType::INT_8:
        return readInt8();
    default:
        throwInvalidType("INT_32", token);
    }
}

int64_t MessagePackParser::unpackInt64()
{
    auto token = readUInt8();

    if (IS_FIXINT(token))
    {
        return token;
    }

    auto type = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::UINT_64:
        return readInt64(true);
    case DataType::INT_64:
        return readInt64();
    case DataType::UINT_32:
        return readInt32(true);
    case DataType::INT_32:
        return readInt32();
    case DataType::UINT_16:
        return readInt16(true);
    case DataType::INT_16:
        return readInt16();
    case DataType::UINT_8:
        return readInt8(true);
    case DataType::INT_8:
        return readInt8();
    default:
        throwInvalidType("INT_64", token);
    }
}

std::string MessagePackParser::unpackString()
{
    auto token      = readUInt8();
    uint32_t length = 0;

    if (IS_FIXSTRING(token))
    {
        length = token & FIXSTRING_LEN_BITS;
    }
    else
    {
        auto type = static_cast<DataType>(token);

        switch (type)
        {
        case DataType::STRING_32:
            length = readUInt32();
            break;
        case DataType::STRING_16:
            length = readUInt16();
            break;
        default:
            throwInvalidType("STRING", token);
        }
    }

    ensureEnoughBytes(length);
    std::string result;
    result.reserve(length);

    for (uint32_t i = 0; i < length; i++)
    {
        result += _input[_position++];
    }

    return result;
}

uint32_t MessagePackParser::unpackMapStart()
{
    auto token = readUInt8();

    if (IS_FIXMAP(token))
    {
        return token & FIXMAP_LEN_BITS;
    }

    auto type = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::MAP_32:
        return readUInt32();
    case DataType::MAP_16:
        return readUInt16();
    default:
        throwInvalidType("MAP", token);
    }
}

ValueMap MessagePackParser::unpackMap()
{
    auto length = unpackMapStart();
    ValueMap result;

    for (uint32_t i = 0; i < length; i++)
    {
        auto key    = unpackString();
        result[key] = unpackValue();
    }

    return result;
}

uint32_t MessagePackParser::unpackArrayStart()
{
    auto token = readUInt8();

    if (IS_FIXARRAY(token))
    {
        return token & FIXARRAY_LEN_BITS;
    }

    auto type = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::ARRAY_32:
        return readUInt32();
    case DataType::ARRAY_16:
        return readUInt16();
    default:
        throwInvalidType("ARRAY", token);
    }
}

ValueVector MessagePackParser::unpackArray()
{
    auto length = unpackArrayStart();
    ValueVector result(length);

    for (uint32_t i = 0; i < length; i++)
    {
        result[i] = unpackValue();
    }

    return result;
}

Value MessagePackParser::unpackValue()
{
    ensureEnoughBytes(1);
    auto token = _input[_position];  // Peek token but don't move the pointer

    if (IS_FIXINT(token))
    {
        _position++;
        return Value(token);
    }
    else if (IS_FIXSTRING(token))
    {
        return Value(unpackString());
    }
    else if (IS_FIXMAP(token))
    {
        return Value(unpackMap());
    }
    else if (IS_FIXARRAY(token))
    {
        return Value(unpackArray());
    }

    auto type = static_cast<DataType>(token);

    switch (type)
    {
    case DataType::NIL:
        _position++;
        return Value::Null;
    case DataType::BOOL_FALSE:
        _position++;
        return Value(false);
    case DataType::BOOL_TRUE:
        _position++;
        return Value(true);
    case DataType::FLOAT_32:
        _position++;
        // TODO: might not work on all architectures
        return Value(std::bit_cast<float>(readUInt32()));
    case DataType::FLOAT_64:
        _position++;
        // TODO: might not work on all architectures
        return Value(std::bit_cast<double>(readUInt64()));
    case DataType::UINT_8:
        _position++;
        return Value(readUInt8());
    case DataType::UINT_16:
        _position++;
        return Value(readUInt16());
    case DataType::UINT_32:
        _position++;
        return Value(readUInt32());
    case DataType::UINT_64:
        _position++;
        return Value(readUInt64());
    case DataType::INT_8:
        _position++;
        return Value(readInt8());
    case DataType::INT_16:
        _position++;
        return Value(readInt16());
    case DataType::INT_32:
        _position++;
        return Value(readInt32());
    case DataType::INT_64:
        _position++;
        return Value(readInt64());
    case DataType::STRING_16:
    case DataType::STRING_32:
        return Value(unpackString());
    case DataType::MAP_16:
    case DataType::MAP_32:
        return Value(unpackMap());
    case DataType::ARRAY_16:
    case DataType::ARRAY_32:
        return Value(unpackArray());
    default:
        throw ParseException(std::format("Unexpected token: 0x{:X}", token));
    }
}

uint8_t MessagePackParser::readUInt8()
{
    ensureEnoughBytes(1);
    return _input[_position++];
}

uint16_t MessagePackParser::readUInt16()
{
    ensureEnoughBytes(2);
    uint16_t result = 0;

    for (uint8_t i = 0; i < 2; i++)
    {
        result <<= 8;
        result |= _input[_position++] & 0xFF;
    }

    return result;
}

uint32_t MessagePackParser::readUInt32()
{
    ensureEnoughBytes(4);
    uint32_t result = 0;

    for (uint8_t i = 0; i < 4; i++)
    {
        result <<= 8;
        result |= _input[_position++] & 0xFF;
    }

    return result;
}

uint64_t MessagePackParser::readUInt64()
{
    ensureEnoughBytes(8);
    uint64_t result = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        result <<= 8;
        result |= _input[_position++] & 0xFF;
    }

    return result;
}

int8_t MessagePackParser::readInt8(bool checkRange)
{
    auto value = readUInt8();

    if (checkRange && value > INT8_MAX)
    {
        throw ParseException("INT_8 value is out of range");
    }

    return static_cast<int8_t>(value);
}

int16_t MessagePackParser::readInt16(bool checkRange)
{
    auto value = readUInt16();

    if (checkRange && value > INT16_MAX)
    {
        throw ParseException("INT_16 value is out of range");
    }

    return value;
}

int32_t MessagePackParser::readInt32(bool checkRange)
{
    auto value = readUInt32();

    if (checkRange && value > INT32_MAX)
    {
        throw ParseException("INT_32 value is out of range");
    }

    return value;
}

int64_t MessagePackParser::readInt64(bool checkRange)
{
    auto value = readUInt64();

    if (checkRange && value > INT64_MAX)
    {
        throw ParseException("INT_64 value is out of range");
    }

    return value;
}

void MessagePackParser::ensureEnoughBytes(size_t length) const
{
    if (_position + length > _length)
    {
        throw ParseException("Unexpected end of input");
    }
}

void MessagePackParser::throwInvalidType(const std::string& expected, uint8_t receivedToken) const
{
    auto message =
        std::format("Encountered unexpected token 0x{:X} while trying to unpack {}", receivedToken, expected);
    throw ParseException(message);
}

}  // namespace opendw::msgpack

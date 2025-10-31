#include "MessagePackPacker.h"

#include "msgpack/MessagePack.h"

#define FIXINT_MIN     -32
#define FIXINT_MAX     127
#define FIXSTRING_MAX  31
#define FIXSTRING_BITS 0b10100000
#define FIXMAP_MAX     15
#define FIXMAP_BITS    0b10000000
#define FIXARRAY_MAX   15
#define FIXARRAY_BITS  0b10010000
#define INT8_SIZE      1
#define INT16_SIZE     2
#define INT32_SIZE     4
#define INT64_SIZE     8

USING_NS_AX;

namespace opendw::msgpack
{

void MessagePackPacker::packNil()
{
    _output.push_back(static_cast<uint8_t>(DataType::NIL));
}

void MessagePackPacker::packBool(bool value)
{
    _output.push_back(static_cast<uint8_t>(value ? DataType::BOOL_TRUE : DataType::BOOL_FALSE));
}

void MessagePackPacker::packFloat(float value)
{
    // TODO: might not work on all architectures
    writeInt32(DataType::FLOAT_32, std::bit_cast<int32_t>(value), INT32_SIZE);
}

void MessagePackPacker::packDouble(double value)
{
    // TODO: might not work on all architectures
    writeInt64(DataType::FLOAT_64, std::bit_cast<int64_t>(value), INT64_SIZE);
}

void MessagePackPacker::packUInt8(uint8_t value)
{
    writeInt8(DataType::UINT_8, value);
}

void MessagePackPacker::packUInt16(uint16_t value)
{
    if (value > UINT8_MAX)
    {
        writeInt16(DataType::UINT_16, value, INT16_SIZE);
    }
    else
    {
        writeInt16(DataType::UINT_8, value, INT8_SIZE);
    }
}

void MessagePackPacker::packUInt32(uint32_t value)
{
    if (value > UINT8_MAX)
    {
        if (value > UINT16_MAX)
        {
            writeInt32(DataType::UINT_32, value, INT32_SIZE);
        }
        else
        {
            writeInt32(DataType::UINT_16, value, INT16_SIZE);
        }
    }
    else
    {
        writeInt32(DataType::UINT_8, value, 1);
    }
}

void MessagePackPacker::packUInt64(uint64_t value)
{
    if (value > UINT16_MAX)
    {
        if (value > UINT32_MAX)
        {
            writeInt64(DataType::UINT_64, value, INT64_SIZE);
        }
        else
        {
            writeInt64(DataType::UINT_32, value, INT32_SIZE);
        }
    }
    else
    {
        if (value > UINT8_MAX)
        {
            writeInt64(DataType::UINT_16, value, INT16_SIZE);
        }
        else
        {
            writeInt64(DataType::UINT_8, value, INT8_SIZE);
        }
    }
}

void MessagePackPacker::packInt8(int8_t value)
{
    if (value < FIXINT_MIN)
    {
        writeInt8(DataType::INT_8, value);
    }
    else
    {
        _output.push_back(static_cast<uint8_t>(value));
    }
}

void MessagePackPacker::packInt16(int16_t value)
{
    if (value < FIXINT_MIN)
    {
        if (value < INT8_MIN)
        {
            writeInt16(DataType::INT_16, value, INT16_SIZE);
        }
        else
        {
            writeInt16(DataType::INT_8, value, INT8_SIZE);
        }
    }
    else
    {
        if (value > INT8_MAX)
        {
            packUInt16(static_cast<uint32_t>(value));
        }
        else
        {
            _output.push_back(static_cast<uint8_t>(value));
        }
    }
}

void MessagePackPacker::packInt32(int32_t value)
{
    if (value < FIXINT_MIN)
    {
        if (value < INT16_MIN)
        {
            writeInt32(DataType::INT_32, value, INT32_SIZE);
        }
        else if (value < INT8_MIN)
        {
            writeInt32(DataType::INT_16, value, INT16_SIZE);
        }
        else
        {
            writeInt32(DataType::INT_8, value, INT8_SIZE);
        }
    }
    else
    {
        if (value > INT8_MAX)
        {
            packUInt32(static_cast<uint32_t>(value));
        }
        else
        {
            _output.push_back(static_cast<uint8_t>(value));
        }
    }
}

void MessagePackPacker::packInt64(int64_t value)
{
    if (value < FIXINT_MIN)
    {
        if (value < INT16_MIN)
        {
            if (value < INT32_MIN)
            {
                writeInt64(DataType::INT_64, value, INT64_SIZE);
            }
            else
            {
                writeInt64(DataType::INT_32, value, INT32_SIZE);
            }
        }
        else
        {
            if (value < INT8_MIN)
            {
                writeInt64(DataType::INT_16, value, INT16_SIZE);
            }
            else
            {
                writeInt64(DataType::INT_8, value, INT8_SIZE);
            }
        }
    }
    else
    {
        if (value > INT8_MAX)
        {
            packUInt64(static_cast<uint64_t>(value));
        }
        else
        {
            _output.push_back(static_cast<uint8_t>(value));
        }
    }
}

void MessagePackPacker::packString(const std::string& value)
{
    auto length = value.length();

    if (length > FIXSTRING_MAX)
    {
        if (length > UINT16_MAX)
        {
            writeInt32(DataType::STRING_32, length, INT32_SIZE);
        }
        else
        {
            writeInt16(DataType::STRING_16, length, INT16_SIZE);
        }
    }
    else
    {
        _output.push_back(static_cast<uint8_t>(length | FIXSTRING_BITS));
    }

    for (auto c : value)
    {
        _output.push_back(static_cast<uint8_t>(c));
    }
}

void MessagePackPacker::packMapStart(uint32_t length)
{
    if (length > FIXMAP_MAX)
    {
        if (length > UINT16_MAX)
        {
            writeInt32(DataType::MAP_32, length, INT32_SIZE);
        }
        else
        {
            writeInt16(DataType::MAP_16, length, INT16_SIZE);
        }
    }
    else
    {
        _output.push_back(static_cast<uint8_t>(length | FIXMAP_BITS));
    }
}

void MessagePackPacker::packMap(const ValueMap& value)
{
    packMapStart(value.size());

    for (const auto& entry : value)
    {
        packString(entry.first);
        packValue(entry.second);
    }
}

void MessagePackPacker::packArrayStart(uint32_t length)
{
    if (length > FIXARRAY_MAX)
    {
        if (length > UINT16_MAX)
        {
            writeInt32(DataType::ARRAY_32, length, INT32_SIZE);
        }
        else
        {
            writeInt16(DataType::ARRAY_16, length, INT16_SIZE);
        }
    }
    else
    {
        _output.push_back(static_cast<uint8_t>(length | FIXARRAY_BITS));
    }
}

void MessagePackPacker::packArray(const ValueVector& value)
{
    packArrayStart(value.size());

    for (const auto& element : value)
    {
        packValue(element);
    }
}

void MessagePackPacker::packValue(const Value& value)
{
    switch (value.getType())
    {
    case Value::Type::NONE:
        packNil();
        break;
    case Value::Type::BOOLEAN:
        packBool(value.asBool());
        break;
    case Value::Type::FLOAT:
        packFloat(value.asFloat());
        break;
    case Value::Type::DOUBLE:
        packDouble(value.asDouble());
        break;
    case Value::Type::INT_I32:
        packInt32(value.asInt());
        break;
    case Value::Type::INT_UI32:
        packUInt32(value.asUint());
        break;
    case Value::Type::INT_I64:
        packInt64(value.asInt64());
        break;
    case Value::Type::INT_UI64:
        packUInt64(value.asUint64());
        break;
    case Value::Type::STRING:
        packString(value.asString());
        break;
    case Value::Type::MAP:
        packMap(value.asValueMap());
        break;
    case Value::Type::VECTOR:
        packArray(value.asValueVector());
        break;
    }
}

void MessagePackPacker::writeInt8(DataType type, int8_t value)
{
    _output.push_back(static_cast<uint8_t>(type));
    _output.push_back(value);
}

void MessagePackPacker::writeInt16(DataType type, int16_t value, uint8_t size)
{
    AX_ASSERT(size > 0 && size <= INT16_SIZE);
    _output.push_back(static_cast<uint8_t>(type));

    for (uint8_t i = 0; i < size; i++)
    {
        uint8_t shift = (size - i - 1) << 3;
        _output.push_back((value >> shift) & 0xFF);
    }
}

void MessagePackPacker::writeInt32(DataType type, int32_t value, uint8_t size)
{
    AX_ASSERT(size > 0 && size <= INT32_SIZE);
    _output.push_back(static_cast<uint8_t>(type));

    for (uint8_t i = 0; i < size; i++)
    {
        uint8_t shift = (size - i - 1) << 3;
        _output.push_back((value >> shift) & 0xFF);
    }
}

void MessagePackPacker::writeInt64(DataType type, int64_t value, uint8_t size)
{
    AX_ASSERT(size > 0 && size <= INT64_SIZE);
    _output.push_back(static_cast<uint8_t>(type));

    for (uint8_t i = 0; i < size; i++)
    {
        uint8_t shift = (size - i - 1) << 3;
        _output.push_back((value >> shift) & 0xFF);
    }
}

}  // namespace opendw::msgpack

#ifndef __MESSAGE_PACK_PACKER_H__
#define __MESSAGE_PACK_PACKER_H__

#include "axmol.h"

namespace opendw::msgpack
{

enum class DataType : uint8_t;

class MessagePackPacker
{
public:
    void packNil();

    void packBool(bool value);

    void packFloat(float value);
    void packDouble(double value);

    void packUInt8(uint8_t value);
    void packUInt16(uint16_t value);
    void packUInt32(uint32_t value);
    void packUInt64(uint64_t value);

    void packInt8(int8_t value);
    void packInt16(int16_t value);
    void packInt32(int32_t value);
    void packInt64(int64_t value);

    void packString(const std::string& value);

    void packMapStart(uint32_t length);
    void packMap(const ax::ValueMap& value);

    void packArrayStart(uint32_t length);
    void packArray(const ax::ValueVector& value);

    void packValue(const ax::Value& value);

    const std::vector<uint8_t>& getOutput() const { return _output; }

private:
    void writeInt8(DataType type, int8_t value);
    void writeInt16(DataType type, int16_t value, uint8_t size);
    void writeInt32(DataType type, int32_t value, uint8_t size);
    void writeInt64(DataType type, int64_t value, uint8_t size);

    std::vector<uint8_t> _output;
};

}  // namespace opendw::msgpack

#endif  // __MESSAGE_PACK_PACKER_H__

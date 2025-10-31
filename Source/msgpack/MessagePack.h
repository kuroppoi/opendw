#ifndef __MESSAGE_PACK_H__
#define __MESSAGE_PACK_H__

#include "msgpack/MessagePackPacker.h"
#include "msgpack/MessagePackParser.h"

namespace opendw::msgpack
{

enum class DataType : uint8_t
{
    NIL        = 0xC0,
    BOOL_FALSE = 0xC2,
    BOOL_TRUE  = 0xC3,
    FLOAT_32   = 0xCA,
    FLOAT_64   = 0xCB,
    UINT_8     = 0xCC,
    UINT_16    = 0xCD,
    UINT_32    = 0xCE,
    UINT_64    = 0xCF,
    INT_8      = 0xD0,
    INT_16     = 0xD1,
    INT_32     = 0xD2,
    INT_64     = 0xD3,
    STRING_16  = 0xDA,
    STRING_32  = 0xDB,
    MAP_16     = 0xDE,
    MAP_32     = 0xDF,
    ARRAY_16   = 0xDC,
    ARRAY_32   = 0xDD
};

}  // namespace opendw::msgpack

#endif  // __MESSAGE_PACK_H__

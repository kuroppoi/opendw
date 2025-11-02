#ifndef __MESSAGE_IDENT_H__
#define __MESSAGE_IDENT_H__

#include <stdint.h>

namespace opendw
{

enum class MessageIdent : uint8_t
{
    AUTHENTICATE  = 1,
    MOVE          = 5,
    BLOCKS        = 16,
    BLOCKS_IGNORE = 25
};

}  // namespace opendw

#endif  // __MESSAGE_IDENT_H__

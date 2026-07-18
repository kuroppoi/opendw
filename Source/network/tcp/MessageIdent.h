#ifndef __MESSAGE_IDENT_H__
#define __MESSAGE_IDENT_H__

#include <stdint.h>

namespace opendw
{

enum class MessageIdent : uint8_t
{
    AUTHENTICATE   = 1,
    MOVE           = 5,
    INVENTORY_USE  = 10,
    BLOCK_MINE     = 11,
    BLOCK_PLACE    = 12,
    INVENTORY_MOVE = 14,
    BLOCKS         = 16,
    CRAFT          = 19,
    BLOCK_USE      = 21,
    ZONE_SEARCH    = 23,
    ZONE_CHANGE    = 24,
    BLOCKS_IGNORE  = 25,
    RESPAWN        = 26,
    DIALOG         = 45
};

}  // namespace opendw

#endif  // __MESSAGE_IDENT_H__

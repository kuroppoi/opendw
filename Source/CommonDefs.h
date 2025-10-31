#ifndef __COMMON_DEFS_H__
#define __COMMON_DEFS_H__

#include <stdint.h>
#include <string>

#define BLOCK_SIZE 80.0F  // 40 if low definition, but opendw only supports HD textures.

/* Macro for creating an object and calling a custom init function with params */
#define CREATE_INIT(__TYPE__, __INITIALIZER__, ...) \
    do                                              \
    {                                               \
        __TYPE__* ret = new __TYPE__();             \
        if (ret->__INITIALIZER__(__VA_ARGS__))      \
        {                                           \
            ret->autorelease();                     \
        }                                           \
        else                                        \
        {                                           \
            AX_SAFE_DELETE(ret);                    \
        }                                           \
        return ret;                                 \
    } while (0)

namespace opendw
{}  // namespace opendw

#endif  // __COMMON_DEFS_H__

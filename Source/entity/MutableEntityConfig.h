#ifndef __MUTABLE_ENTITY_CONFIG_H__
#define __MUTABLE_ENTITY_CONFIG_H__

#include "entity/EntityConfig.h"

namespace opendw
{

/*
 * CLASS: MutableEntityConfig : EntityConfig @ 0x10031B090
 */
class MutableEntityConfig : public EntityConfig
{
public:
    /* FUNC: MutableEntityConfig::inheritFromConfig: @ 0x100120EA1 */
    void inheritFromConfig(EntityConfig* config);
};

}  // namespace opendw

#endif  // __MUTABLE_ENTITY_CONFIG_H__

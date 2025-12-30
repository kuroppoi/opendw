#ifndef __ENTITY_ANIMATED_AVATAR_H__
#define __ENTITY_ANIMATED_AVATAR_H__

#include "entity/EntityAnimatedHuman.h"

namespace opendw
{

/*
 * CLASS: EntityAnimatedAvatar : EntityAnimatedHuman @ 0x10031E628
 */
class EntityAnimatedAvatar : public EntityAnimatedHuman
{
public:
    /* FUNC: EntityAnimatedAvatar::isAvatar @ 0x10017CA30 */
    bool isAvatar() const override { return true; }
};

}  // namespace opendw

#endif  // __ENTITY_ANIMATED_AVATAR_H__

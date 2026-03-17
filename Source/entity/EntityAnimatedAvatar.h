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

    /* FUNC: EntityAnimatedAvatar::setFootColliderCount: @ 0x10017D489 */
    void setFootColliderCount(int64_t count);

    /* FUNC: EntityAnimatedAvatar::footColliderCount @ 0x10017D677 */
    int64_t getFootColliderCount() const { return _footColliderCount; }

    /* FUNC: EntityAnimatedAvatar::setHeadColliderCount: @ 0x10017D699 */
    void setHeadColliderCount(int64_t count) { _headColliderCount = count; }

    /* FUNC: EntityAnimatedAvatar::headColliderCount @ 0x10017D688 */
    int64_t getHeadColliderCount() const { return _headColliderCount; }

private:
    int64_t _footColliderCount;  // EntityAnimatedAvatar::footColliderCount @ 0x1003152B0
    int64_t _headColliderCount;  // EntityAnimatedAvatar::headColliderCount @ 0x1003152C8
};

}  // namespace opendw

#endif  // __ENTITY_ANIMATED_AVATAR_H__

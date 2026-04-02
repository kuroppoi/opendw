#ifndef __ENTITY_ANIMATED_AVATAR_H__
#define __ENTITY_ANIMATED_AVATAR_H__

#include "entity/EntityAnimatedHuman.h"

namespace opendw
{

class BaseBlock;
class Item;

/*
 * CLASS: EntityAnimatedAvatar : EntityAnimatedHuman @ 0x10031E628
 */
class EntityAnimatedAvatar : public EntityAnimatedHuman
{
public:
    /* FUNC: EntityAnimatedAvatar::isAvatar @ 0x10017CA30 */
    bool isAvatar() const override { return true; }

    /* FUNC: EntityAnimatedAvatar::setAlive: @ 0x10017D286 */
    void setAlive(bool alive) override;

    /* FUNC: EntityAnimatedAvatar::walkOn: @ 0x10017D4B8 */
    void walkOnBlock(BaseBlock* block);

    /* FUNC: EntityAnimatedAvatar::playWalkSfx @ 0x10017D518 */
    void playWalkSfx(bool force = false);

    /* FUNC: EntityAnimatedAvatar::setFootColliderCount: @ 0x10017D489 */
    void setFootColliderCount(int64_t count);

    /* FUNC: EntityAnimatedAvatar::footColliderCount @ 0x10017D677 */
    int64_t getFootColliderCount() const { return _footColliderCount; }

    /* FUNC: EntityAnimatedAvatar::setHeadColliderCount: @ 0x10017D699 */
    void setHeadColliderCount(int64_t count) { _headColliderCount = count; }

    /* FUNC: EntityAnimatedAvatar::headColliderCount @ 0x10017D688 */
    int64_t getHeadColliderCount() const { return _headColliderCount; }

    /* FUNC: EntityAnimatedAvatar::walkOnItem @ 0x10017D6BB */
    Item* getFeetItem() const { return _feetItem; }

    /* FUNC: EntityAnimatedAvatar::walkOnMod @ 0x10017D6CC */
    uint8_t getFeetMod() const { return _feetMod; }

    double getLastFootstepSoundAt() const { return _lastFootstepSoundAt; }

private:
    int64_t _footColliderCount;   // EntityAnimatedAvatar::footColliderCount @ 0x1003152B0
    int64_t _headColliderCount;   // EntityAnimatedAvatar::headColliderCount @ 0x1003152C8
    Item* _feetItem;              // EntityAnimatedAvatar::walkOnItem @ 0x100315270
    uint8_t _feetMod;             // EntityAnimatedAvatar::walkOnMod @ 0x1003152B8
    double _lastFootstepSoundAt;  // EntityAnimatedAvatar::lastFootstepSoundAt @ 0x100315278
};

}  // namespace opendw

#endif  // __ENTITY_ANIMATED_AVATAR_H__

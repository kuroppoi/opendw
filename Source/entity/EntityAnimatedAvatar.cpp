#include "EntityAnimatedAvatar.h"

#include "base/Item.h"
#include "zone/BaseBlock.h"
#include "AudioManager.h"

#define WALK_SFX_INTERVAL 0.15

USING_NS_AX;

namespace opendw
{

void EntityAnimatedAvatar::setAlive(bool alive)
{
    if (_alive != alive && !alive)
    {
        _mainSkeleton->setAnimation(0, "death-fall-back", false);
        _currentAnimation = -1;  // BUGFIX: Otherwise it might not play the proper animation after respawning
        // TODO: update physical
    }

    EntityAnimated::setAlive(alive);
}

void EntityAnimatedAvatar::walkOnBlock(BaseBlock* block)
{
    _feetItem = block->getFrontItem();
    _feetMod  = block->getFrontMod();
    playWalkSfx();
}

void EntityAnimatedAvatar::playWalkSfx(bool force)
{
    if (_feetItem && (force || utils::gettime() > _lastFootstepSoundAt + WALK_SFX_INTERVAL))
    {
        AudioManager::getInstance()->playSfx("footsteps", _feetItem->getMaterial(), 0.3F, 0.15F);
        _lastFootstepSoundAt = utils::gettime();
    }
}

void EntityAnimatedAvatar::setFootColliderCount(int64_t count)
{
    if (_footColliderCount != count)
    {
        _footColliderCount = count;
        setGrounded(count > 0);
    }
}

}  // namespace opendw

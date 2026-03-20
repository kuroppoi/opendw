#include "EntityAnimatedAvatar.h"

namespace opendw
{

void EntityAnimatedAvatar::setFootColliderCount(int64_t count)
{
    if (_footColliderCount != count)
    {
        _footColliderCount = count;
        setGrounded(count > 0);
    }
}

}  // namespace opendw

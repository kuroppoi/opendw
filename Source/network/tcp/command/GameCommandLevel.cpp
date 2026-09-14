#include "GameCommandLevel.h"

#include "util/MapUtil.h"
#include "GameManager.h"

USING_NS_AX;

namespace opendw
{

void GameCommandLevel::run()
{
    // TODO: set player level
    auto level    = _data[0].asInt();
    auto subtitle = std::format("You are now level {}.", level);
    GameManager::getInstance()->notify(NotificationType::BIG_ALERT,
                                       Value(map_util::mapOf("t", "Level Up!", "t2", subtitle)));
}

}  // namespace opendw

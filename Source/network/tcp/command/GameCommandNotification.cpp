#include "GameCommandNotification.h"

#include "GameManager.h"

namespace opendw
{

void GameCommandNotification::run()
{
    auto& data = _data[0];
    auto type  = static_cast<NotificationType>(_data[1].asUint());
    GameManager::getInstance()->notify(type, data);
}

}  // namespace opendw

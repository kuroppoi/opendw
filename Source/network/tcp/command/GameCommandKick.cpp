#include "GameCommandKick.h"

#include "GameManager.h"

namespace opendw
{

void GameCommandKick::run()
{
    auto message         = _data[0].asString();
    auto shouldReconnect = _data[1].asBool();
    GameManager::getInstance()->kickPlayer(message, shouldReconnect);
}

}  // namespace opendw

#include "GameCommandConfigure.h"

#include "zone/WorldZone.h"
#include "GameManager.h"

namespace opendw
{

void GameCommandConfigure::initWithData(const uint8_t* data, size_t length)
{
    GameCommand::initWithData(data, length);
    AXLOGI("Configuration packed size is {}", length);
}

void GameCommandConfigure::run()
{
    // Order:
    // 1. Preconfigure player
    // 2. Configure game
    // 3. Configure zone
    // 4. Configure player

    // TODO: implement further
    auto playerEntityId = _data[0].asInt();
    AXLOGI("Player's entity ID is {}", playerEntityId);
    auto game = GameManager::getInstance();
    game->configure(_data[2].asValueMap());
    game->getZone()->configure(_data[3].asValueMap());
}

}  // namespace opendw

#include "GameCommandConfigure.h"

#include "zone/WorldZone.h"
#include "GameManager.h"
#include "Player.h"

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

    auto game     = GameManager::getInstance();
    auto player   = game->getPlayer();
    auto entityId = _data[0].asInt();
    player->setEntityId(entityId);
    AXLOGI("Player's entity ID is {}", entityId);
    player->preconfigure(_data[1].asValueMap());
    game->configure(_data[2].asValueMap());
    game->getZone()->configure(_data[3].asValueMap());
    player->configure(_data[1].asValueMap());
}

}  // namespace opendw

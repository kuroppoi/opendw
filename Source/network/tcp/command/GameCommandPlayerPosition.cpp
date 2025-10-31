#include "GameCommandPlayerPosition.h"

#include "graphics/WorldRenderer.h"
#include "zone/WorldZone.h"
#include "GameManager.h"
#include "Player.h"

namespace opendw
{

void GameCommandPlayerPosition::run()
{
    // TODO: finish
    auto game   = GameManager::getInstance();
    auto player = game->getPlayer();
    auto x = _data[0].asFloat();
    auto y = _data[1].asFloat();
    player->setPosition({x, -y});  // TODO: use collider position
    game->getZone()->getWorldRenderer()->updateViewport(1.0F);
}

}  // namespace opendw

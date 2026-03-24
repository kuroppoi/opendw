#include "GameCommandPlayerPosition.h"

#include "base/Player.h"
#include "graphics/WorldRenderer.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"
#include "GameManager.h"

namespace opendw
{

void GameCommandPlayerPosition::run()
{
    // TODO: finish
    auto game   = GameManager::getInstance();
    auto player = game->getPlayer();
    auto x = _data[0].asFloat();
    auto y = _data[1].asFloat();
    player->setPosition({(x + 0.5F) * BLOCK_SIZE, (y + 0.5F) * -BLOCK_SIZE});
    game->getZone()->getWorldRenderer()->updateViewport(1.0F);
}

}  // namespace opendw

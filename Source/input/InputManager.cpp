#include "InputManager.h"

#include "GameManager.h"
#include "Player.h"

USING_NS_AX;

namespace opendw
{

bool InputManager::initWithGame(GameManager* game)
{
    if (!Node::init())
    {
        return false;
    }

    _game   = game;
    _player = game->getPlayer();
    return true;
}

}  // namespace opendw

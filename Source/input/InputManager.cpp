#include "InputManager.h"

#include "base/InventoryItem.h"
#include "base/Item.h"
#include "base/Player.h"
#include "gui/GameGui.h"
#include "zone/BaseBlock.h"
#include "GameManager.h"

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

void InputManager::enterGame()
{
    _gameGui = GameGui::getMain();
}

void InputManager::useActiveHotbarItem(bool use, const Point& point)
{
    // TODO: finish
    auto mining = false;

    if (use)
    {
        _player->useActiveHotbarItem(point);

        if (auto item = _player->getActiveHotbarItem())
        {
            mining = item->getItem()->isMiningTool();
        }
    }
    else
    {
        _player->setUsingHotbarItem(nullptr);
    }

    if (!mining)
    {
        auto block = _player->getMiningBlock();

        if (block)
        {
            block->cancelMining();
        }
    }
}

}  // namespace opendw

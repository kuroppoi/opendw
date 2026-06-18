#include "GameCommandPlayerInventory.h"

#include "base/ContainerType.h"
#include "base/GameConfig.h"
#include "base/Item.h"
#include "base/Player.h"
#include "zone/WorldZone.h"
#include "GameManager.h"

USING_NS_AX;

namespace opendw
{

static ContainerType getContainerTypeForName(const std::string& name)
{
    if (!name.empty())
    {
        switch (name[0])
        {
        case 'i':
            return ContainerType::INVENTORY;
        case 'h':
            return ContainerType::HOTBAR;
        case 'a':
            return ContainerType::ACCESSORY;
        case 'z':
            return ContainerType::HIDDEN;
        }
    }

    return ContainerType::NONE;
}

void GameCommandPlayerInventory::run()
{
    auto& items = _data[0].asValueMap();
    auto game   = GameManager::getInstance();
    auto player = game->getPlayer();
    std::set<int64_t> categories;  // Categories that need to be rearranged

    for (auto& entry : items)
    {
        auto code = stoi(entry.first);
        auto item = game->getConfig()->getItemForCode(code);

        if (!item)
        {
            AXLOGW("[GameCommandPlayerInventory] Unknown item code: {}", code);
            continue;
        }

        auto& details  = entry.second.asValueVector();
        auto quantity  = details[0].asInt();
        auto container = getContainerTypeForName(details[1].asString());
        auto slot      = details[2].asInt();
        player->setInventory(item, quantity, container, slot);
        categories.insert(item->getInventoryPosition().category);
    }

    for (auto category : categories)
    {
        player->arrangeInventory(category);
    }

    if (game->getZone()->getState() != WorldZone::State::ACTIVE)
    {
        player->updateAccessories();
    }
}

}  // namespace opendw

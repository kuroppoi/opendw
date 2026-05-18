#include "InventoryItem.h"

#include "base/Item.h"
#include "base/Player.h"
#include "gui/GameGui.h"
#include "network/tcp/MessageIdent.h"
#include "GameManager.h"
#include "CommonDefs.h"

namespace opendw
{

InventoryItem* InventoryItem::createWithItem(Item* item, int64_t quantity, ContainerType container, int64_t slot)
{
    CREATE_INIT(InventoryItem, initWithItem, item, quantity, container, slot);
}

bool InventoryItem::initWithItem(Item* item, int64_t quantity, ContainerType container, int64_t slot)
{
    _item      = item;
    _quantity  = quantity;
    _container = container;
    _slot      = slot;
    return true;
}

void InventoryItem::update()
{
    if (_quantity < 1)
    {
        _container = ContainerType::NONE;
    }
    else if (_container == ContainerType::NONE || _slot == -1)
    {
        // TODO: moveToInitialPosition();
    }

    GameGui::getMain()->updateInventoryItem(this);
}

static const char* getServerContainerForType(ContainerType type)
{
    switch (type)
    {
    case ContainerType::INVENTORY:
        return "i";
    case ContainerType::HOTBAR:
        return "h";
    case ContainerType::ACCESSORY:
        return "a";
    default:
        return nullptr;
    }
}

void InventoryItem::updateServer()
{
    if (auto container = getServerContainerForType(_container))
    {
        GameManager::getInstance()->sendMessage(MessageIdent::INVENTORY_MOVE, _item->getCode(), container, _slot);
    }
}

bool InventoryItem::shouldNotifyOnIncrease() const
{
    return _item->getCode() != 512;  // ground/earth
}

void InventoryItem::moveToContainer(ContainerType container, int64_t slot, int64_t category)
{
    // TODO: update accessories

    if (_container == container && _slot == slot && _category == category)
    {
        return;
    }

    _container = container;
    _slot      = slot;
    _category  = category;
    update();
    updateServer();

    // Update active item if item moved from or to the active hotbar slot
    auto player = Player::getMain();

    if ((container == ContainerType::HOTBAR && slot == player->getActiveHotbarSlot()) ||
        player->getActiveHotbarItem() == this)
    {
        player->updateActiveHotbarItem();
    }
}

void InventoryItem::setQuantity(int64_t quantity)
{
    // TODO: show feedback if new quantity is greater

    if (_quantity != quantity)
    {
        _quantity = quantity;
        update();
    }
}

void InventoryItem::setPosition(int64_t slot, int64_t category)
{
    if (category == -1)
    {
        category = _category;
    }

    if (_slot != slot || _category != category)
    {
        _slot     = slot;
        _category = category;
        update();
    }
}

}  // namespace opendw

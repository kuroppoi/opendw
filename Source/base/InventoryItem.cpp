#include "InventoryItem.h"

#include "base/Item.h"
#include "base/Player.h"
#include "entity/EntityAnimatedAvatar.h"
#include "graphics/WorldRenderer.h"
#include "gui/GameGui.h"
#include "network/tcp/MessageIdent.h"
#include "zone/WorldZone.h"
#include "GameManager.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

InventoryItem* InventoryItem::createWithItem(Item* item, int64_t quantity, ContainerType container, int64_t slot)
{
    CREATE_INIT(InventoryItem, initWithItem, item, quantity, container, slot);
}

bool InventoryItem::initWithItem(Item* item, int64_t quantity, ContainerType container, int64_t slot)
{
    _item          = item;
    _quantity      = quantity;
    _container     = container;
    _slot          = slot;
    _positionDirty = true;
    return true;
}

void InventoryItem::update()
{
    auto gui    = GameGui::getMain();
    auto player = Player::getMain();

    if (_quantity < 1)
    {
        _previousContainer = _container;
        _container         = ContainerType::NONE;
        _positionDirty     = true;
    }
    else if (_container == ContainerType::NONE || _slot == -1)
    {
        _previousContainer = _container;
        gui->updateInventoryItem(this);  // It might still be in an ItemContainer, so we make sure to remove it first
        moveToInitialPosition();
        _positionDirty = true;
    }

    if (_positionDirty && WorldZone::getMain()->getState() == WorldZone::State::ACTIVE)
    {
        // Update active item if item moved from or to the active hotbar slot
        if ((_container == ContainerType::HOTBAR && _slot == player->getActiveHotbarSlot()) ||
            player->getActiveHotbarItem() == this)
        {
            player->updateActiveHotbarItem();
        }

        // Update accessories if item moved from or to the accessory or hidden item list
        if (_previousContainer == ContainerType::ACCESSORY || _container == ContainerType::ACCESSORY ||
            _previousContainer == ContainerType::HIDDEN || _container == ContainerType::HIDDEN)
        {
            player->updateAccessories();
        }
    }

    gui->updateInventoryItem(this);
    _positionDirty = false;
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

void InventoryItem::emit(ssize_t count)
{
    auto position = Player::getMain()->getPosition();
    position.y += BLOCK_SIZE * 2.0F;
    WorldRenderer::getMain()->emitItemAnimation(_item, position, count);
}

bool InventoryItem::shouldNotifyOnIncrease() const
{
    return _item->getCode() != 512;  // ground/earth
}

void InventoryItem::moveToContainer(ContainerType container, int64_t slot, int64_t category)
{
    if (_quantity < 1)
    {
        return;
    }

    _previousContainer = _container;

    if (_container == container)
    {
        if (_slot == slot && _category == category)
        {
            return;
        }
        else if (_container == ContainerType::INVENTORY)
        {
            update();
            return;
        }
    }

    _container     = container;
    _slot          = slot;
    _category      = category;
    _positionDirty = true;
    update();
    updateServer();
}

void InventoryItem::moveToInitialPosition()
{
    _category = 0;  // Managed by arrangeInventory

    if (WorldZone::getMain()->getState() != WorldZone::State::ACTIVE)
    {
        _container = ContainerType::INVENTORY;
        _slot      = 0;
        return;
    }

    auto player = Player::getMain();

    // Try accessory bar
    if (_item->isEquippableAccessory())
    {
        _slot = player->getNextInventorySlot(ContainerType::ACCESSORY);

        if (_slot != -1)
        {
            _container = ContainerType::ACCESSORY;
            updateServer();
            return;
        }
    }

    // Try hotbar
    _slot = player->getNextInventorySlot(ContainerType::HOTBAR);

    if (_slot != -1)
    {
        _container = ContainerType::HOTBAR;
        updateServer();
    }
    else
    {
        _container = ContainerType::INVENTORY;
        _slot      = 0;
        player->arrangeInventory(_item);
    }
}

void InventoryItem::setQuantity(int64_t quantity)
{
    if (_quantity == quantity)
    {
        return;
    }

    // Notify if quantity increased
    if (quantity > _quantity)
    {
        auto gained = quantity - _quantity;

        if (shouldNotifyOnIncrease())
        {
            auto text = std::format("+{} {}", gained, _item->getTitle());
            Player::getMain()->getAvatar()->emote(text, Color3B::WHITE, true, true);
        }

        emit(MIN(10, gained));
    }

    _quantity = quantity;
    update();
}

void InventoryItem::setPosition(int64_t slot, int64_t category)
{
    if (category == -1)
    {
        category = _category;
    }

    if (_slot != slot || _category != category)
    {
        _previousContainer = _container;
        _slot              = slot;
        _category          = category;
        _positionDirty     = true;
        update();
    }
}

}  // namespace opendw

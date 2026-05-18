#include "InventoryItemSprite.h"

#include "base/InventoryItem.h"
#include "base/Item.h"
#include "gui/GameGui.h"
#include "util/MathUtil.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

InventoryItemSprite* InventoryItemSprite::createWithItem(InventoryItem* item)
{
    CREATE_INIT(InventoryItemSprite, initWithItem, item);
}

bool InventoryItemSprite::initWithItem(InventoryItem* item)
{
    if (!ItemSprite::initWithItem(item->getItem()))
    {
        return false;
    }

    _inventoryItem = item;

    // 0x10006EDB6: Create quantity label
    _quantityLabel = Label::createWithBMFont("inventory-font.fnt", "0", TextHAlignment::RIGHT);
    _quantityLabel->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    _quantityLabel->setPosition(_contentSize.width, -6.0F);  // NOTE: Originally -4
    _quantityLabel->setScale(_contentSize.width / (math_util::getScaledWidth(_quantityLabel) * 4.0F));
    addChild(_quantityLabel, 3);

    // 0x10006EE79: Create quantity label shadow
    _quantityLabelShadow = Label::createWithBMFont("inventory-font.fnt", "0", TextHAlignment::RIGHT);
    _quantityLabelShadow->setAnchorPoint(_quantityLabel->getAnchorPoint());
    _quantityLabelShadow->setPosition(_quantityLabel->getPositionX() - 1.0F, _quantityLabel->getPositionY() + 1.0F);
    _quantityLabelShadow->setScale(_quantityLabel->getScale());
    _quantityLabelShadow->setColor(Color3B::BLACK);
    addChild(_quantityLabelShadow, 2);
    updateQuantity();
    setCascadeOpacityEnabled(true);
    return true;
}

void InventoryItemSprite::activate()
{
    GameGui::getMain()->setActiveItemSprite(this);
}

void InventoryItemSprite::updateQuantity()
{
    auto quantity = _inventoryItem->getQuantity();
    auto text     = quantity < 10000 ? std::to_string(quantity) : std::format("{}K", MIN(999, quantity / 1000));
    _quantityLabel->setString(text);
    _quantityLabelShadow->setString(text);
}

}  // namespace opendw

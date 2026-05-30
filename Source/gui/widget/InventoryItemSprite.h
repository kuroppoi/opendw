#ifndef __INVENTORY_ITEM_SPRITE_H__
#define __INVENTORY_ITEM_SPRITE_H__

#include "axmol.h"

#include "gui/widget/ItemSprite.h"

namespace opendw
{

class InventoryItem;
class Item;

/*
 * CLASS: InventoryItemSprite : CCSprite @ 0x100317288
 */
class InventoryItemSprite : public ItemSprite
{
public:
    /* FUNC: InventoryItemSprite::spriteWithItem: @ 0x10006EC4B */
    static InventoryItemSprite* createWithItem(InventoryItem* item);

    bool initWithItem(InventoryItem* item);

    /* FUNC: InventoryItemSprite::activate @ 0x10006F228 */
    void activate() override;

    /* FUNC: InventoryItemSprites::tooltipComponents @ 0x10006F293 */
    void getTooltipComponents(std::vector<ax::Node*>& output) override;

    /* FUNC: InventoryItemSprite::updateCount: @ 0x10006EF4F */
    void updateQuantity();

    /* FUNC: InventoryItemSprite::inventoryItem @ 0x10006F47B */
    InventoryItem* getInventoryItem() const { return _inventoryItem; }

private:
    InventoryItem* _inventoryItem;    // InventoryItemSprite::inventoryItem @ 0x100311B60
    ax::Label* _quantityLabel;        // InventoryItemSprite::quantityLabel @ 0x100311B50
    ax::Label* _quantityLabelShadow;  // InventoryItemSprite::quantityLabelShadow @ 0x100311B58
};

}  // namespace opendw

#endif  // __INVENTORY_ITEM_SPRITE_H__

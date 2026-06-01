#ifndef __INVENTORY_ITEM_H__
#define __INVENTORY_ITEM_H__

#include "axmol.h"

#include "base/ContainerType.h"

namespace opendw
{

class Item;

/*
 * CLASS: InventoryItem : NSObject @ 0x1003170A8
 */
class InventoryItem : public ax::Object
{
public:
    static InventoryItem* createWithItem(Item* item, int64_t quantity, ContainerType container, int64_t slot = -1);

    /* FUNC: InventoryItem::initWithDictionary: @ 0x10006CFB5 */
    bool initWithItem(Item* item, int64_t quantity, ContainerType container, int64_t slot = -1);

    /* FUNC: InventoryItem::update @ 0x10006D72A */
    void update();

    /* FUNC: InventoryItem::updateServer @ 0x10006D7B5 */
    void updateServer();
    
    /* FUNC: InventoryItem::emit @ 0x10006D983 */
    void emit(ssize_t count);

    /* FUNC: InventoryItem::shouldNotifyOnIncrease @ 0x10006D290 */
    bool shouldNotifyOnIncrease() const;

    /* FUNC: InventoryItem::moveToContainer:position: @ 0x10006D33A */
    void moveToContainer(ContainerType container, int64_t slot, int64_t category = 0);

    /* FUNC: InventoryItem::moveToInitialPosition @ 0x10006D529 */
    void moveToInitialPosition();

    /* FUNC: InventoryItem::isInInventory @ 0x10006D89D */
    bool isInInventory() const { return _container == ContainerType::INVENTORY; }

    /* FUNC: InventoryItem::item @ 0x10006DB43 */
    Item* getItem() const { return _item; }

    /* FUNC: InventoryItem::setCount: @ 0x10006D0EF */
    void setQuantity(int64_t quantity);

    /* FUNC: InventoryItem::count @ 0x10006DB71 */
    int64_t getQuantity() const { return _quantity; }

    /* FUNC: InventoryItem::container @ 0x10006DB82 */
    ContainerType getContainer() const { return _container; }

    /* FUNC: InventoryItem::setPosition: @ 0x10006D31D */
    void setPosition(int64_t slot, int64_t category = -1);

    /* FUNC: InventoryItem::position @ 0x10006DB93 */
    int64_t getSlot() const { return _slot; }
    int64_t getCategory() const { return _category; }

private:
    Item* _item;               // InventoryItem::item @ 0x100311B08
    int64_t _quantity;         // InventoryItem::count @ 0x100311AF0
    ContainerType _container;  // InventoryItem::container @ 0x100311B00
    int64_t _slot;             // InventoryItem::position @ 0x100311AF8
    int64_t _category;
};

}  // namespace opendw

#endif  // __INVENTORY_ITEM_H__

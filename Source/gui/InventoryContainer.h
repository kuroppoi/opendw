#ifndef __INVENTORY_CONTAINER_H__
#define __INVENTORY_CONTAINER_H__

#include "axmol.h"

namespace opendw
{

class GameGui;

/*
 * CLASS: InventoryContainer : CCNode @ 0x100317058
 */
class InventoryContainer : public ax::Node
{
public:
    static InventoryContainer* createWithGui(GameGui* gui, const std::string& name, int columns, int rows);

    /* FUNC: InventoryContainer::initWithGui:name:cols:rows: @ 0x10006A793 */
    bool initWithGui(GameGui* gui, const std::string& name, int columns, int rows);

    /* FUNC: InventoryContainer::updatePositioning @ 0x10006B955 */
    void updatePositioning();

    /* FUNC: InventoryContainer::pointAtItemIndex: @ 0x10006BCB2 */
    ax::Point getPointAtItemIndex(int index) const;

    /* FUNC: InventoryContainer::slotSprites @ 0x10006CF09 */
    const std::vector<ax::Sprite*>& getSlotSprites() const { return _slotSprites; }

private:
    GameGui* _gameGui;                      // InventoryContainer::gameGui @ 0x100311A68
    std::string _name;                      // InventoryContainer::name @ 0x100311A70
    int _columns;                           // InventoryContainer::cols @ 0x100311A78
    int _rows;                              // InventoryContainer::rows @ 0x100311A80
    ax::SpriteBatchNode* _guiBatch;         // InventoryContainer::guiBatch @ 0x100311AB8
    ax::SpriteBatchNode* _inventoryBatch;   // InventoryContainer::inventoryBatch @ 0x100311AC0
    std::vector<ax::Sprite*> _slotSprites;  // InventoryContainer::slotSprites @ 0x100311A98
    float _itemMargin;                      // InventoryContainer::itemMargin @ 0x100311AA0
    float _itemSize;                        // InventoryContainer::itemSize @ 0x100311AA8
};

}  // namespace opendw

#endif  // __INVENTORY_CONTAINER_H__

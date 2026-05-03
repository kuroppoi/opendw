#include "InventoryContainer.h"

#include "gui/GameGui.h"

#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

InventoryContainer* InventoryContainer::createWithGui(GameGui* gui, const std::string& name, int columns, int rows)
{
    CREATE_INIT(InventoryContainer, initWithGui, gui, name, columns, rows);
}

bool InventoryContainer::initWithGui(GameGui* gui, const std::string& name, int columns, int rows)
{
    if (!Node::init())
    {
        return false;
    }

    _gameGui    = gui;
    _name       = name;
    _columns    = columns;
    _rows       = rows;
    _itemMargin = gui->getItemMargin();
    _itemSize   = gui->getItemSize();
    _guiBatch   = SpriteBatchNode::create("guiv2.png");
    addChild(_guiBatch, 1);
    _inventoryBatch = SpriteBatchNode::create("inventory+hd2.png");
    addChild(_inventoryBatch, 2);

    // 0x10006ACA1: Create slot sprites
    _slotSprites.reserve((ssize_t)rows * columns);

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < columns; x++)
        {
            auto sprite = Sprite::createWithSpriteFrameName("inventory-slot");
            _inventoryBatch->addChild(sprite);
            _slotSprites.push_back(sprite);
        }
    }

    auto width  = (_columns - 1) * _itemMargin + _columns * _itemSize;
    auto height = (_rows - 1) * _itemMargin + _rows * _itemSize;
    setContentSize({width, height});
    updatePositioning();
    return true;
}

void InventoryContainer::updatePositioning()
{
    // 0x10006B966: Position slot sprites
    for (ssize_t i = 0; i < _slotSprites.size(); i++)
    {
        auto sprite = _slotSprites[i];
        sprite->setPosition(getPointAtItemIndex(i));
    }
}

Point InventoryContainer::getPointAtItemIndex(int index) const
{
    AXASSERT(index >= 0 && index < _slotSprites.size(), "Invalid item index");
    auto x      = index % _columns;
    auto y      = index / _columns;
    auto pointX = _itemSize * (x + 0.5F) + x * _itemMargin;
    auto pointY = _contentSize.height - (_itemSize * (y + 0.5F) + y * _itemMargin);
    return Point(pointX, pointY);
}

}  // namespace opendw

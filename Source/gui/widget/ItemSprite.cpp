#include "ItemSprite.h"

#include "base/Item.h"
#include "gui/widget/ItemContainer.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

ItemSprite* ItemSprite::createWithItem(Item* item)
{
    CREATE_INIT(ItemSprite, initWithItem, item);
}

bool ItemSprite::initWithItem(Item* item)
{
    if (!Sprite::initWithSpriteFrame(item->getInventoryFrame()))
    {
        return false;
    }

    _item = item;
    _initialScale = 1.0F;
    setColor(item->getSpriteColor());  // BUGFIX: Crystal block colors
    return true;
}

void ItemSprite::removeFromContainer()
{
    if (_container.pointer)
    {
        _container.pointer->removeSprite(this);
    }
}

void ItemSprite::setScale(float scale)
{
    _initialScale = scale;  // NOTE: This relies on the fact that scale actions do not call setScale()
    Sprite::setScale(scale);
}

}  // namespace opendw

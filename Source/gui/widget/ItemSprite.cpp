#include "ItemSprite.h"

#include "base/Item.h"
#include "gui/widget/ItemContainer.h"
#include "CommonDefs.h"

#define USE_SPRITE_COLOR 1

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
#if USE_SPRITE_COLOR
    setColor(item->getSpriteColor());
#endif
    return true;
}

void ItemSprite::removeFromContainer()
{
    if (_container.pointer)
    {
        _container.pointer->removeSprite(this);
    }
}

}  // namespace opendw

#include "ItemSprite.h"

#include "base/Item.h"
#include "CommonDefs.h"

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
    return true;
}

}  // namespace opendw

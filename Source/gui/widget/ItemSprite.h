#ifndef __ITEM_SPRITE_H__
#define __ITEM_SPRITE_H__

#include "axmol.h"

namespace opendw
{

class Item;

class ItemSprite : public ax::Sprite
{
public:
    static ItemSprite* createWithItem(Item* item);

    bool initWithItem(Item* item);

    Item* getItem() const { return _item; }

protected:
    Item* _item;
};

}  // namespace opendw

#endif  // __ITEM_SPRITE_H__

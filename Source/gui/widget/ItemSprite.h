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

    void removeFromContainer();

    virtual void activate() {}

    virtual void getTooltipComponents(std::vector<ax::Node*>& output) {};

    Item* getItem() const { return _item; }

protected:
    Item* _item;

private:
    friend class ItemContainer;

    // Internal info for ItemContainer
    struct Container
    {
        ItemContainer* pointer;
        int64_t category;
        int64_t slot;
    } _container{};
};

}  // namespace opendw

#endif  // __ITEM_SPRITE_H__

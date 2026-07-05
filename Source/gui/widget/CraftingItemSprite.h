#ifndef __CRAFTING_ITEM_SPRITE_H__
#define __CRAFTING_ITEM_SPRITE_H__

#include "gui/widget/ItemSprite.h"

namespace opendw
{

class Recipe;

/*
 * CLASS: CraftingItemSprite : CCSprite @ 0x10031A168
 */
class CraftingItemSprite : public ItemSprite
{
public:
    virtual ~CraftingItemSprite() override;

    static CraftingItemSprite* createWithRecipe(Recipe* recipe, bool ownsRecipe = false);

    /* FUNC: CraftingItemSprite::initWithRecipe:index: @ 0x1000F3680 */
    bool initWithRecipe(Recipe* recipe, bool ownsRecipe = false);

    /* FUNC: CraftingItemSprite::activate @ 0x1000F3842 */
    void activate() override;

    /* FUNC: CraftingItemSprite::attemptMake @ 0x1000F39B0 */
    void attemptMake();

    /* FUNC: CraftingItemSprite::tooltipComponents @ 0x1000F3B13 */
    void getTooltipComponents(std::vector<ax::Node*>& output) override;

    /* FUNC: CraftingItemSprite::recipe @ 0x1000F4343 */
    Recipe* getRecipe() const { return _recipe; }

    /* FUNC: CraftingItemSprite::setMakeable: @ 0x1000F37B5 */
    void setMakeable(bool makeable);

private:
    Recipe* _recipe;  // CraftingItemSprite::recipe @ 0x1003133F8
    bool _makeable;   // CraftingItemSprite::makeable @ 0x100313418
    bool _ownsRecipe;
};

}  // namespace opendw

#endif  // __CRAFTING_ITEM_SPRITE_H__

#ifndef __CRAFTING_CONTAINER_H__
#define __CRAFTING_CONTAINER_H__

#include "event/EventListenerContainer.h"
#include "gui/widget/ItemContainer.h"

namespace opendw
{

class CraftingItemSprite;
class GameGui;
class Item;

/*
 * CLASS: CraftingContainer : InventoryContainer @ 0x10031A078
 */
class CraftingContainer : public ItemContainer, EventListenerContainer
{
public:
    static CraftingContainer* createWithGui(GameGui* gui, int32_t cols, int32_t rows);

    bool initWithGui(GameGui* gui, int32_t cols, int32_t rows);

    /* FUNC: CraftingContainer::onEnter @ 0x1000EFF34 */
    void onEnter() override;

    /* FUNC: CraftingContainer::onExit @ 0x1000F0A12 */
    void onExit() override;

    /* FUNC: CraftingContainer::processRecipes @ 0x1000F006A */
    void processRecipes();

    /* FUNC: CraftingContainer::updateAllRecipes @ 0x1000F0671 */
    void updateAllRecipes();

    /* FUNC: CraftingContainer::updateRecipesForIngredient: @ 0x1000F0832 */
    void updateRecipesForIngredient(Item* item);

    /* FUNC: CraftingContainer::inventoryDidChange: @ 0x1000F044F */
    void onInventoryChanged(Item* item);

    /* FUNC: CraftingContainer::playerSkillDidChange: @ 0x1000F0626 */
    void onPlayerSkillChanged();

private:
    std::unordered_map<uint16_t, std::vector<CraftingItemSprite*>>
        _spritesByIngredient;  // CraftingContainer::spritesByIngredient @ 0x100313378
};

}  // namespace opendw

#endif  // __CRAFTING_CONTAINER_H__

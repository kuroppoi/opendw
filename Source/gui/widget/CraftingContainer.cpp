#include "CraftingContainer.h"

#include "base/Item.h"
#include "base/Player.h"
#include "base/Recipe.h"
#include "event/EventNames.h"
#include "gui/widget/CraftingItemSprite.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"

namespace opendw
{

CraftingContainer* CraftingContainer::createWithGui(GameGui* gui, int32_t cols, int32_t rows)
{
    CREATE_INIT(CraftingContainer, initWithGui, gui, cols, rows);
}

bool CraftingContainer::initWithGui(GameGui* gui, int32_t cols, int32_t rows)
{
    if (!ItemContainer::initWithGui(gui, cols, rows))
    {
        return false;
    }

    setOrganizable(false);
    return true;
}

void CraftingContainer::onEnter()
{
    ItemContainer::onEnter();
    addEventListener(events::kInventoryChanged, EVENT_CALLBACK(Item*, onInventoryChanged));
    addEventListener(events::kPlayerSkillChanged, AX_CALLBACK_0(CraftingContainer::onPlayerSkillChanged, this));
}

void CraftingContainer::onExit()
{
    removeEventListeners();
    ItemContainer::onExit();
}

void CraftingContainer::processRecipes()
{
    for (auto child : _itemSpriteNode->getChildren())
    {
        auto sprite = static_cast<CraftingItemSprite*>(child);
        sprite->setMakeable(false);

        // Populate sprites by ingredient lookup table
        for (auto& ingredient : sprite->getRecipe()->getIngredients())
        {
            auto& sprites = _spritesByIngredient[ingredient.item->getCode()];
            sprites.push_back(sprite);
        }
    }
}

void CraftingContainer::updateAllRecipes()
{
    for (auto child : _itemSpriteNode->getChildren())
    {
        auto sprite = static_cast<CraftingItemSprite*>(child);
        sprite->setMakeable(Player::getMain()->canMakeRecipe(sprite->getRecipe()));
    }
}

void CraftingContainer::updateRecipesForIngredient(Item* item)
{
    auto it = _spritesByIngredient.find(item->getCode());

    if (it != _spritesByIngredient.end())
    {
        auto& sprites = (*it).second;

        for (auto sprite : sprites)
        {
            sprite->setMakeable(Player::getMain()->canMakeRecipe(sprite->getRecipe()));
        }
    }
}

void CraftingContainer::onInventoryChanged(Item* item)
{
    if (item)
    {
        updateRecipesForIngredient(item);
    }
    else  // Update all recipes if general inventory update
    {
        updateAllRecipes();
    }
}

void CraftingContainer::onPlayerSkillChanged()
{
    if (WorldZone::getMain()->getState() == WorldZone::State::ACTIVE)
    {
        updateAllRecipes();
    }
}

}  // namespace opendw

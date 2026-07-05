#include "Recipe.h"

#include "base/GameConfig.h"
#include "base/Item.h"
#include "CommonDefs.h"

#define RARITY_WARN_THRESHOLD 3  // Minimum item rarity at which the player gets a confirmation screen when crafting

USING_NS_AX;

namespace opendw
{

Recipe* Recipe::createWithItem(Item* item)
{
    CREATE_INIT(Recipe, initWithItem, item);
}

bool Recipe::initWithItem(Item* item)
{
    auto& ingredients = item->getCraftingIngredients();

    if (ingredients.empty())
    {
        return false;
    }

    _item       = item;
    _quantity   = item->getCraftingQuantity();
    auto config = GameConfig::getMain();

    // 0x1000F0C48: Configure crafting ingredients
    for (auto& ingredient : ingredients)
    {
        switch (ingredient.getType())
        {
        case Value::Type::STRING:
        {
            if (auto item = config->getItemForName(ingredient.asString()))
            {
                _ingredients.push_back({item, 1});
                _warn |= item->getRarity() >= RARITY_WARN_THRESHOLD;
            }

            break;
        }
        case Value::Type::VECTOR:
        {
            auto& array = ingredient.asValueVector();

            if (auto item = config->getItemForName(array[0].asString()))
            {
                auto quantity = array[1].asInt64();
                _ingredients.push_back({item, quantity});
                _warn |= item->getRarity() >= RARITY_WARN_THRESHOLD;
            }

            break;
        }
        }
    }

    // Configure crafting helpers
    for (auto& helper : item->getCraftingHelpers())
    {
        auto& array = helper.asValueVector();

        if (auto item = config->getItemForName(array[0].asString()))
        {
            auto quantity = array[1].asInt64();
            _helpers.push_back({item, quantity});
        }
    }

    return true;
}

}  // namespace opendw

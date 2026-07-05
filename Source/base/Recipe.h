#ifndef __RECIPE_H__
#define __RECIPE_H__

#include "axmol.h"

namespace opendw
{

class Item;

/*
 * CLASS: Recipe : NSObject @ 0x10031A0C8
 *
 * Model containing information about a crafting recipe for an item.
 */
class Recipe : public ax::Object
{
public:
    struct Ingredient
    {
        Item* item;
        int64_t quantity;
    };

    /* FUNC: Recipe::recipeWithItemName: @ 0x1000F0A7C */
    static Recipe* createWithItem(Item* item);

    /* FUNC: Recipe::initWithItem: @ 0x1000F0B15 */
    bool initWithItem(Item* item);

    /* FUNC: Recipe::item @ 0x1000F0F28 */
    Item* getItem() const { return _item; }

    /* FUNC: Recipe::quantity @ 0x1000F0F84 */
    int64_t getQuantity() const { return _quantity; }

    /* FUNC: Recipe::warn @ 0x1000F0FA6 */
    bool shouldWarn() const { return _warn; }

    /* FUNC: Recipe::ingredients @ 0x1000F0F56 */
    const std::vector<Ingredient>& getIngredients() const { return _ingredients; }

    /* @return A vector of crafting helpers (workshop items) required for this recipe. */
    const std::vector<Ingredient>& getHelpers() const { return _helpers; }

private:
    Item* _item;                           // Recipe::item @ 0x100313380
    int64_t _quantity;                     // Recipe::quantity @ 0x100313398
    bool _warn;                            // Recipe::warn @ 0x100313388
    std::vector<Ingredient> _ingredients;  // Recipe::ingredients @ 0x10031339
    std::vector<Ingredient> _helpers;
};

}  // namespace opendw

#endif  // __RECIPE_H__

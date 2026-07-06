#include "CraftingItemSprite.h"

#include "base/Item.h"
#include "base/Player.h"
#include "base/Recipe.h"
#include "util/ColorUtil.h"
#include "util/MathUtil.h"
#include "AudioManager.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

CraftingItemSprite::~CraftingItemSprite()
{
    if (_ownsRecipe)
    {
        AX_SAFE_RELEASE(_recipe);
    }
}

CraftingItemSprite* CraftingItemSprite::createWithRecipe(Recipe* recipe, bool ownsRecipe)
{
    CREATE_INIT(CraftingItemSprite, initWithRecipe, recipe, ownsRecipe);
}

bool CraftingItemSprite::initWithRecipe(Recipe* recipe, bool ownsRecipe)
{
    if (!ItemSprite::initWithItem(recipe->getItem()))
    {
        return false;
    }

    if (ownsRecipe)
    {
        AX_SAFE_RETAIN(recipe);
    }

    _recipe     = recipe;
    _ownsRecipe = ownsRecipe;
    return true;
}

void CraftingItemSprite::activate()
{
    // TODO: implement warning dialog

    if (_makeable)
    {
        attemptMake();
    }
}

void CraftingItemSprite::attemptMake()
{
    auto sfx = "error";

    if (Player::getMain()->makeRecipe(_recipe))
    {
        stopAllActions();
        setScale(_initialScale);
        auto scaleUp   = ScaleBy::create(0.1F, 1.222F);
        auto scaleDown = ScaleTo::create(0.1F, _initialScale);
        runAction(Sequence::createWithTwoActions(scaleUp, scaleDown));
        sfx = "craft";
    }

    AudioManager::getInstance()->playSfx(sfx, 1.0F, 0.0F, 0.3F);
}

/* NOTE: See function 0x100069735 */
static std::vector<Node*> getItemComponents(const std::vector<Recipe::Ingredient>& ingredients, float* widthPtr)
{
    std::vector<Node*> result;
    result.reserve(ingredients.size());
    auto width = 0.0F;

    for (auto& ingredient : ingredients)
    {
        auto item   = ingredient.item;
        auto sprite = Sprite::createWithSpriteFrame(item->getInventoryFrame());

        if (!sprite)
        {
            AXLOGW("[CraftingItemSprite] No inventory frame for crafting ingredient: {}", item->getName());
            continue;
        }

        // Set sprite properties
        math_util::scaleToSize(sprite, Size::ONE * 30.0F, true);
        sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        sprite->setPositionY(math_util::getScaledHeight(sprite) * 0.5F);

        // Create quantity label
        auto quantityLabel = Label::createWithBMFont("console.fnt", std::format("x{}", ingredient.quantity));
        quantityLabel->setColor(Color3B::BLACK);
        quantityLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        quantityLabel->setPosition(sprite->getBoundingBox().getMaxX() + 10.0F, sprite->getPositionY());
        quantityLabel->setScale(0.8F);

        // Create title label
        auto titleLabel = Label::createWithBMFont("console.fnt", item->getTitle());
        titleLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        titleLabel->setColor(color_util::hexToColor("5C5C5C"));
        titleLabel->setPosition(quantityLabel->getBoundingBox().getMaxX() + 10.0F, quantityLabel->getPositionY());
        titleLabel->setScale(0.7F);

        // Create item node
        auto node = Node::create();
        node->addChild(sprite);
        node->addChild(quantityLabel);
        node->addChild(titleLabel);
        node->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        node->setContentSize({titleLabel->getBoundingBox().getMaxX(), sprite->getBoundingBox().getMaxY()});
        result.push_back(node);
        width = MAX(width, node->getContentSize().width);
    }

    // Use a consistent width for all item nodes
    for (auto node : result)
    {
        auto& contentSize = node->getContentSize();
        node->setContentSize({width, contentSize.height});
    }

    if (widthPtr)
    {
        *widthPtr = width;
    }

    return result;
}

void CraftingItemSprite::getTooltipComponents(std::vector<Node*>& output)
{
    // 0x1000F41EF: Create quantity label
    auto quantity = _recipe->getQuantity();

    if (quantity > 1)
    {
        auto quantityLabel = Label::createWithBMFont("console.fnt", std::format("Makes {}", _recipe->getQuantity()));
        quantityLabel->setColor(color_util::hexToColor("645014"));
        quantityLabel->setScale(0.7F);
        output.push_back(quantityLabel);
    }

    // 0x1000F40D7: Create ingredient list
    auto totalWidth  = 0.0F;
    auto ingredients = getItemComponents(_recipe->getIngredients(), &totalWidth);
    output.insert(output.end(), ingredients.begin(), ingredients.end());

    // 0x1000F3DF4: Create crafting helper list
    auto& helpers = _recipe->getHelpers();

    if (!helpers.empty())
    {
        // 0x1000F4077: Create divider
        auto divider = Sprite::createWithSpriteFrameName("white");
        divider->setColor(color_util::hexToColor("645014"));
        output.push_back(divider);

        // 0x1000F3FF3: Create label
        auto label = Label::createWithBMFont("console.fnt", "Workshop Requirements");
        label->setColor(color_util::hexToColor("645014"));
        label->setScale(0.7F);
        output.push_back(label);

        // Create item list
        auto width = 0.0F;
        auto list  = getItemComponents(helpers, &width);
        totalWidth = MAX(width, MAX(totalWidth, math_util::getScaledWidth(label)));
        math_util::scaleToSize(divider, {totalWidth, 1.0F});
        output.insert(output.end(), list.begin(), list.end());
    }

    // 0x1000F3C2F: Create crafting skill label if not skilled enough to craft
    auto item = _recipe->getItem();

    if (!Player::getMain()->isSkilledToCraft(item))
    {
        std::string skill = item->getCraftingSkill();  // Mutable copy
        skill[0]          = std::toupper(skill[0]);
        auto text         = std::format("Level {} {}", item->getCraftingSkillLevel(), skill);
        auto label        = Label::createWithBMFont("console.fnt", text);
        label->setScale(0.6F);
        label->setColor(color_util::hexToColor("FF3232"));
        output.push_back(label);
    }
}

void CraftingItemSprite::setMakeable(bool makeable)
{
    _makeable = makeable;
    setOpacity(makeable ? 255 : 51);
}

}  // namespace opendw

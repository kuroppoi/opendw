#include "CraftingPanel.h"

#include "ui/UIScale9Sprite.h"

#include "base/ContainerType.h"
#include "base/GameConfig.h"
#include "base/Item.h"
#include "base/Recipe.h"
#include "gui/widget/CraftingContainer.h"
#include "gui/widget/CraftingItemSprite.h"
#include "gui/GameGui.h"
#include "util/ColorUtil.h"
#include "util/MapUtil.h"

USING_NS_AX;

namespace opendw
{

void CraftingPanel::onEnter()
{
    Node::onEnter();

    if (_loaded)
    {
        return;
    }

    auto gui          = GameGui::getMain();
    auto panelPadding = gui->getPanelPadding();

    // Load category info from recipe sections
    auto config   = GameConfig::getMain();
    auto sections = config->getRecipeSections();
    std::vector<std::string> categoryIcons;
    std::vector<std::string> categoryNames;

    for (auto& section : sections)
    {
        auto& category = section.asValueMap();
        categoryIcons.push_back(map_util::getString(category, "icon"));
        auto name = map_util::getString(category, "name", "unknown");

        if (!name.empty())
        {
            name[0] = toupper(name[0]);
        }

        categoryNames.push_back(name);
    }

    // 0x100181660: Create crafting container
    auto craftingContainer = CraftingContainer::createWithGui(gui, gui->getCraftingCols(), gui->getCraftingRows());
    craftingContainer->setOpaqueSlots(false);
    craftingContainer->setPosition(panelPadding + 17.0F, panelPadding + 26.0F);
    craftingContainer->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    craftingContainer->setDynamicPaging(true);
    craftingContainer->setCategories(categoryIcons);
    craftingContainer->setCategoryChangeCallback(
        [=](int64_t category) { _categoryLabel->setString(categoryNames[category]); });
    craftingContainer->updateLayout();
    addChild(craftingContainer, 1);
    gui->setItemContainerForType(ContainerType::CRAFTING, craftingContainer);

    // Create crafting container background
    auto background = ui::Scale9Sprite::createWithSpriteFrameName("panels/parchment");
    background->setContentSize(craftingContainer->getContentSize() + Vec2::UNIT_X * 30.0F + Vec2::UNIT_Y * 56.0F);
    background->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    background->setPosition(panelPadding, panelPadding);
    addChild(background);

    // 0x10018141A: Create crafting label
    auto craftingLabel = Label::createWithBMFont("console.fnt", "Crafting:");
    craftingLabel->setScale(0.8F);
    craftingLabel->setColor(color_util::hexToColor("FFDC0A"));
    craftingLabel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    craftingLabel->setPosition(panelPadding + 32.0F, background->getBoundingBox().getMaxY() + 3.0F);
    addChild(craftingLabel);

    // 0x100181567: Create category label
    _categoryLabel = Label::createWithBMFont("console.fnt", "Construction");
    _categoryLabel->setScale(craftingLabel->getScale());
    _categoryLabel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _categoryLabel->setPosition(craftingLabel->getBoundingBox().getMaxX() + 10.0F, craftingLabel->getPositionY());
    addChild(_categoryLabel);

    // 0x1001818DC: Populate crafting container
    for (ssize_t i = 0; i < sections.size(); i++)
    {
        auto& section = sections[i].asValueMap();
        auto& items   = map_util::getArray(section, "items");
        int64_t slot  = 0;

        for (auto& element : items)
        {
            auto item = config->getItemForName(element.asString());

            if (item && item->isCraftable())
            {
                auto recipe = Recipe::createWithItem(item);  // Managed by CraftingItemSprite
                auto sprite = CraftingItemSprite::createWithRecipe(recipe, true);
                craftingContainer->addSprite(sprite, slot++, i);
            }
        }
    }

    craftingContainer->processRecipes();
    _loaded = true;
}

}  // namespace opendw

#include "InventoryPanel.h"

#include "ui/UIScale9Sprite.h"

#include "base/ContainerType.h"
#include "base/GameConfig.h"
#include "gui/widget/ItemContainer.h"
#include "gui/GameGui.h"
#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"

#define ACCESSORY_COLUMNS 5
#define ACCESSORY_PAGES   4

USING_NS_AX;

namespace opendw
{

void InventoryPanel::onEnter()
{
    Node::onEnter();

    if (_loaded)
    {
        return;
    }

    auto gui          = GameGui::getMain();
    auto panelPadding = gui->getPanelPadding();

    // Load inventory category info from game config
    auto config      = GameConfig::getMain();
    auto& categories = map_util::getArray(config->getData(), "inventory");
    std::vector<std::string> categoryIcons;
    std::vector<std::string> categoryNames;

    for (auto& element : categories)
    {
        auto& category = element.asValueMap();
        categoryIcons.push_back(map_util::getString(category, "icon"));
        auto name = map_util::getString(category, "name", "unknown");

        if (!name.empty())
        {
            name[0] = toupper(name[0]);
        }

        categoryNames.push_back(name);
    }

    // 0x1001810A7: Create inventory container
    auto inventoryContainer = ItemContainer::createWithGui(gui, gui->getInventoryCols(), gui->getInventoryRows());
    inventoryContainer->setOpaqueSlots(false);
    inventoryContainer->setPosition(panelPadding + 17.0F, panelPadding + 26.0F);
    inventoryContainer->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    inventoryContainer->setDynamicPaging(true);
    inventoryContainer->setCategories(categoryIcons);
    inventoryContainer->setCategoryChangeCallback(
        [=](int64_t category) { _categoryLabel->setString(categoryNames[category]); });
    inventoryContainer->updateLayout();
    addChild(inventoryContainer, 1, "inventory");
    gui->setItemContainerForType(ContainerType::INVENTORY, inventoryContainer);

    // Create inventory container background
    auto background = ui::Scale9Sprite::createWithSpriteFrameName("panels/parchment");
    background->setContentSize(inventoryContainer->getContentSize() + Vec2::UNIT_X * 30.0F + Vec2::UNIT_Y * 56.0F);
    background->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    background->setPosition(panelPadding, panelPadding);
    addChild(background);

    // 0x100180ECB: Create inventory label
    auto inventoryLabel = Label::createWithBMFont("console.fnt", "Inventory:");
    inventoryLabel->setScale(0.8F);
    inventoryLabel->setColor(color_util::hexToColor("FFDC0A"));
    inventoryLabel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    inventoryLabel->setPosition(panelPadding + 14.0F, background->getBoundingBox().getMaxY() + 3.0F);
    addChild(inventoryLabel);

    // 0x100180F99: Create category label
    _categoryLabel = Label::createWithBMFont("console.fnt", "Resources");
    _categoryLabel->setScale(inventoryLabel->getScale());
    _categoryLabel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _categoryLabel->setPosition(inventoryLabel->getBoundingBox().getMaxX() + 10.0F, inventoryLabel->getPositionY());
    addChild(_categoryLabel);

    // 0x1001809D2: Create accessory container
    auto accessoryContainer = ItemContainer::createWithGui(gui, ACCESSORY_COLUMNS, 1);
    accessoryContainer->setPageCount(ACCESSORY_PAGES);
    accessoryContainer->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    accessoryContainer->setPosition(_contentSize.width * 0.5F, inventoryLabel->getBoundingBox().getMaxY() + 20.0F);
    accessoryContainer->updateLayout();
    addChild(accessoryContainer, 1, "accessories");
    gui->setItemContainerForType(ContainerType::ACCESSORY, accessoryContainer);

    // 0x1001807F1: Create accessories label
    auto accessoryLabel = Label::createWithBMFont("console.fnt", "Accessories");
    accessoryLabel->setScale(0.8F);
    accessoryLabel->setColor(color_util::hexToColor("FFDC0A"));
    accessoryLabel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    accessoryLabel->setPosition(accessoryContainer->getBoundingBox().getMinX(),
                                accessoryContainer->getBoundingBox().getMaxY());
    addChild(accessoryLabel, 1);

    // 0x100180BE4: Create accessory container background
    auto accessoryBackground = Sprite::createWithSpriteFrameName("white-33-percent");
    accessoryBackground->setColor(Color3B::BLACK);
    accessoryBackground->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    accessoryBackground->setPosition(accessoryContainer->getPosition());
    auto height = math_util::getScaledHeight(accessoryContainer) + math_util::getScaledHeight(accessoryLabel) + 4.0F;
    math_util::scaleToSize(accessoryBackground, {_contentSize.width - panelPadding, height});
    addChild(accessoryBackground);

    _loaded = true;
}

}  // namespace opendw

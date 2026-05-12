#include "InventoryPanel.h"

#include "gui/GameGui.h"
#include "util/ColorUtil.h"
#include "util/MathUtil.h"

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
    auto itemMargin   = gui->getItemMargin();
    auto itemSize     = gui->getItemSize();

    // 0x1001807F1: Create accessories label
    auto accessoryLabel = Label::createWithBMFont("console.fnt", "Accessories");
    accessoryLabel->setScale(0.8F);
    accessoryLabel->setColor(color_util::hexToColor("FFDC0A"));
    accessoryLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    accessoryLabel->setPosition((itemMargin + itemSize) * 0.5F + panelPadding * 1.25F,
                                _contentSize.height - panelPadding - 3.0F);
    addChild(accessoryLabel, 1);

    // 0x100180BE4: Create accessory container background
    auto accessoryBackground = Sprite::createWithSpriteFrameName("white-33-percent");
    accessoryBackground->setColor(Color3B::BLACK);
    accessoryBackground->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    accessoryBackground->setPosition(panelPadding * 0.5F, _contentSize.height - panelPadding);
    math_util::scaleToSize(accessoryBackground,
                           {_contentSize.width - panelPadding,
                            itemSize + itemMargin * 4.0F + math_util::getScaledHeight(accessoryLabel) + 30.0F});
    addChild(accessoryBackground);

    // 0x100180ECB: Create inventory label
    auto inventoryLabel = Label::createWithBMFont("console.fnt", "Inventory:");
    inventoryLabel->setScale(accessoryLabel->getScale());
    inventoryLabel->setColor(accessoryLabel->getColor());
    inventoryLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    inventoryLabel->setPosition(panelPadding + 14.0F, accessoryBackground->getBoundingBox().getMinY() - 10.0F);
    accessoryBackground->getBoundingBox().getMinY();
    addChild(inventoryLabel);

    // 0x100180F99: Create category label
    auto categoryLabel = Label::createWithBMFont("console.fnt", "Resources");
    categoryLabel->setScale(inventoryLabel->getScale());
    categoryLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    categoryLabel->setPosition(inventoryLabel->getBoundingBox().getMaxX() + 10.0F, inventoryLabel->getPositionY());
    addChild(categoryLabel);

    _loaded = true;
}

}  // namespace opendw

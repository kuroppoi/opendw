#include "GameGuiWindow.h"

#include "event/EventNames.h"
#include "gui/CraftingPanel.h"
#include "gui/GameGui.h"
#include "gui/InventoryPanel.h"
#include "CommonDefs.h"

#define FADE_SPEED 0.0678F

USING_NS_AX;

namespace opendw
{

GameGuiWindow* GameGuiWindow::createWithGui(GameGui* gui, const Size& size)
{
    CREATE_INIT(GameGuiWindow, initWithGui, gui, size);
}

bool GameGuiWindow::initWithGui(GameGui* gui, const Size& size)
{
    if (!initWithStyle("v2-opaquer/brass"))
    {
        return false;
    }

    _gui             = gui;
    _activePanel     = nullptr;
    _activePanelType = PanelType::NONE;
    setContentSize(size);
    setBorderScale(0.8F);
    setCascadeOpacityEnabled(true);
    setOpacity(0);
    _inventoryPanel = InventoryPanel::create();
    addPanel(_inventoryPanel);
    _craftingPanel = CraftingPanel::create();
    addPanel(_craftingPanel);
    auto borderPanel = Panel::createWithStyle("v2-transparent/brass");
    borderPanel->setContentSize(size);
    borderPanel->setBorderScale(_borderScale);
    addChild(borderPanel, 10);
    return true;
}

void GameGuiWindow::addPanel(Node* node)
{
    node->setCascadeOpacityEnabled(true);
    node->setContentSize(_contentSize);
    node->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    node->setPosition(Point::ZERO);
    node->setVisible(false);
    addChild(node, 9);
}

void GameGuiWindow::toggle(PanelType type)
{
    if (_activePanelType == type)
    {
        hide();
    }
    else
    {
        show(type);
    }
}

void GameGuiWindow::show(PanelType type)
{
    if (_activePanelType == type)
    {
        return;
    }

    auto alignmentChanged = getAlignmentForType(_activePanelType) != getAlignmentForType(type);
    auto shouldFadeIn     = _activePanelType == PanelType::NONE || alignmentChanged;
    _inventoryPanel->setVisible(false);
    _craftingPanel->setVisible(false);
    _activePanel = getPanelForType(type);

    if (_activePanel)
    {
        _activePanel->setVisible(true);
    }

    _activePanelType = type;
    updatePosition();

    if (shouldFadeIn)
    {
        stopAllActions();
        setScale(0.888F);
        auto fadeIn  = FadeIn::create(FADE_SPEED);
        auto scaleTo = ScaleTo::create(FADE_SPEED, 1.0F);
        runAction(Spawn::createWithTwoActions(fadeIn, scaleTo));
    }

    _eventDispatcher->dispatchCustomEvent(events::kGuiWindowChangedPanel);
    setVisible(true);
    // TODO: send event message to server
}

void GameGuiWindow::hide()
{
    _activePanelType = PanelType::NONE;
    _activePanel     = nullptr;
    stopAllActions();
    auto fadeOut  = FadeOut::create(FADE_SPEED);
    auto scaleTo  = ScaleTo::create(FADE_SPEED, 0.888F);
    auto delay    = DelayTime::create(FADE_SPEED);
    auto callFunc = CallFuncN::create([](Node* node) { node->setVisible(false); });
    auto sequence = Sequence::create({delay, callFunc});
    runAction(Spawn::create({fadeOut, scaleTo, sequence}));
    _eventDispatcher->dispatchCustomEvent(events::kGuiWindowChangedPanel);
    // TODO: send event message to server
}

void GameGuiWindow::updatePosition()
{
    auto alignRight = getAlignmentForType(_activePanelType) == 1;
    setAnchorPoint(alignRight ? Point::ANCHOR_TOP_RIGHT : Point::ANCHOR_TOP_LEFT);
    setPosition(_gui->getGuiWindowPosition(alignRight));
}

Node* GameGuiWindow::getPanelForType(PanelType type) const
{
    switch (type)
    {
    case PanelType::INVENTORY:
        return _inventoryPanel;
    case PanelType::CRAFTING:
        return _craftingPanel;
    default:
        return nullptr;
    }
}

int GameGuiWindow::getAlignmentForType(PanelType type) const
{
    switch (type)
    {
    case PanelType::PROFILE:
    case PanelType::SETTINGS:
    case PanelType::FEED:
        return -1;
    default:
        return 1;
    };
}

}  // namespace opendw

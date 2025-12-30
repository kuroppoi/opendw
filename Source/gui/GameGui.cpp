#include "GameGui.h"

#include "gui/Panel.h"
#include "gui/SpriteButton.h"
#include "gui/TeleportPanel.h"
#include "util/AxUtil.h"
#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
#include "CommonDefs.h"
#include "GameManager.h"

USING_NS_AX;

namespace opendw
{

GameGui* GameGui::createWithZone(WorldZone* zone)
{
    CREATE_INIT(GameGui, initWithZone, zone);
}

bool GameGui::initWithZone(WorldZone* zone)
{
    if (!Node::init())
    {
        return false;
    }

    _zone     = zone;
    _gameMenu = nullptr;

    // Create announcements node
    _announcementsNode = Node::create();
    addChild(_announcementsNode, 10);

    // Create teleport panel
    _teleportPanel = TeleportPanel::create();
    _teleportPanel->setVisible(false);
    addChild(_teleportPanel, 15);

    // Create hud buttons node
    _hudButtonsNode = Node::create();
    _hudButtonsNode->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
    _hudButtonsNode->setScale(0.5F);
    addChild(_hudButtonsNode, 8);

    // Create world button
    _worldButton = SpriteButton::createWithBackground("hud/bubble-top", "hud/bubble-icon-world");
    _worldButton->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
    _worldButton->setCallback([=]() { showTeleportInterface(nullptr); });
    _hudButtonsNode->addChild(_worldButton);

    AXLOGI("[GameGui] Initialized");
    sMain = this;
    return true;
}

void GameGui::onEnter()
{
    Node::onEnter();
    _eventListeners.push_back(_eventDispatcher->addCustomEventListener(RenderViewImpl::EVENT_WINDOW_RESIZED,
                                                                       AX_CALLBACK_0(GameGui::onWindowResized, this)));
    _eventListeners.push_back(_eventDispatcher->addCustomEventListener("bigAlert", [=](EventCustom* event) {
        auto& data = *static_cast<Value*>(event->getUserData());
        showBigAlert(data);
    }));
    onWindowResized();
}

void GameGui::onExit()
{
    for (auto listener : _eventListeners)
    {
        _eventDispatcher->removeEventListener(listener);
    }

    _eventListeners.clear();
    Node::onExit();
}

void GameGui::ready()
{
    // TODO: implement
}

void GameGui::toggleGameMenu()
{
    if (_gameMenu)
    {
        _gameMenu->removeFromParent();
        _gameMenu = nullptr;
        return;
    }

    // 0x100064DA2: Create gradient
    _gameMenu = LayerGradient::create(color_util::hexToColor4("42424282"), color_util::hexToColor4("00000082"));
    addChild(_gameMenu, 21);

    // 0x100064E3E: Create leave button
    auto leaveButton = MenuItemLabel::create(Label::createWithBMFont("menu.fnt", "Leave game"));
    leaveButton->setScale(0.6F);
    leaveButton->setCallback([=](Object*) {
        AudioManager::getInstance()->playButtonSfx();
        toggleGameMenu();
        GameManager::getInstance()->leaveGame();
    });

    // 0x100064EC0: Create cancel button
    auto cancelButton = MenuItemLabel::create(Label::createWithBMFont("menu.fnt", "Cancel"));
    cancelButton->setScale(0.6F);
    cancelButton->setCallback([=](Object*) {
        AudioManager::getInstance()->playButtonSfx();
        toggleGameMenu();
    });

    // 0x100064DC9: Create menu panel
    auto panel = Panel::createWithStyle("v2-opaquer/brass");
    panel->setContentSize({600.0F, 200.0F});
    panel->setPosition(_director->getWinSize() * 0.5F);
    panel->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _gameMenu->addChild(panel, 1);

    // 0x100064F38: Create menu
    auto menu = Menu::createWithArray({leaveButton, cancelButton});
    menu->alignItemsVerticallyWithPadding(10.0F);
    menu->setPosition(panel->getContentSize() * 0.5F + Vec2::UNIT_Y * 10.0F);
    panel->addChild(menu, 2);
}

void GameGui::showTeleportInterface(BaseBlock* block)
{
    // TODO: finish
    _teleportPanel->showFromBlock(block);
}

void GameGui::hideTeleportInterface()
{
    // TODO: finish
    _teleportPanel->setVisible(false);
}

void GameGui::showTeleportZones(const std::string& type, const std::vector<ZoneSearchInfo>& data)
{
    _teleportPanel->showZones(type, data);
}

void GameGui::showBigAlert(const std::string& title, const std::string& subtitle)
{
    // TODO: finish
    _announcementsNode->removeAllChildren();  // TODO: should fade out and remove all children
    auto& winSize = _director->getWinSize();

    // 0x1000616E0: Create title label
    auto titleLabel = Label::createWithBMFont("menu.fnt", title);
    titleLabel->setPosition(winSize.width * 0.5F, winSize.height * 0.75F);
    titleLabel->setScale(0.4F);
    titleLabel->setOpacity(0);
    _announcementsNode->addChild(titleLabel);
    ax_util::runFadeSequence(titleLabel, 0.2F, 2.5F, 1.0F);
    titleLabel->runAction(ScaleBy::create(3.7F, 1.2F));

    // 0x1000618FC: Create subtitle label
    if (!subtitle.empty())
    {
        auto subtitleLabel = Label::createWithBMFont("console-shadow.fnt", subtitle);
        subtitleLabel->setPosition(titleLabel->getPositionX(),
                                   titleLabel->getPositionY() - math_util::getScaledHeight(titleLabel) - 10.0F);
        subtitleLabel->setScale(0.75F);
        subtitleLabel->setColor(color_util::hexToColor("E6E6E6"));
        subtitleLabel->setOpacity(0);
        _announcementsNode->addChild(subtitleLabel);
        ax_util::runFadeSequence(subtitleLabel, 0.2F, 2.5F, 1.0F);
        subtitleLabel->runAction(ScaleBy::create(3.7F, 1.2F));
    }
}

void GameGui::showBigAlert(const Value& value)
{
    if (value.getType() == Value::Type::MAP)
    {
        auto& map     = value.asValueMap();
        auto title    = map_util::getString(map, "t");
        auto subtitle = map_util::getString(map, "t2");
        showBigAlert(title, subtitle);
    }
    else
    {
        // Assume string
        showBigAlert(value.asString());
    }
}

void GameGui::onWindowResized()
{
    auto viewOffset  = _director->getVisibleOrigin();
    auto visibleSize = _director->getVisibleSize();
    auto left        = viewOffset.x;
    auto bottom      = viewOffset.y;
    auto right       = visibleSize.width + left;
    auto top         = visibleSize.height + bottom;
    auto smallScreen = visibleSize.width <= 1024.0F;

    // Update elements
    _hudButtonsNode->setPosition(right - 200.0F, top);
}

}  // namespace opendw

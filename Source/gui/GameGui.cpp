#include "GameGui.h"

#include "base/GameConfig.h"
#include "base/Player.h"
#include "entity/EntityAnimatedAvatar.h"
#include "event/EventNames.h"
#include "gui/widget/IconBar.h"
#include "gui/widget/MultiLabel.h"
#include "gui/widget/Panel.h"
#include "gui/widget/SpriteButton.h"
#include "gui/TeleportPanel.h"
#include "util/AxUtil.h"
#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
#include "CommonDefs.h"
#include "GameManager.h"

#define GUI_SCALE          1.0F  // TOOD: should be a setting
#define HUD_BUTTON_OPACITY 230   // TODO: should be a setting
#define HUD_BUTTON_SCALE   0.5F
#define PANEL_MARGIN       10.0F
#define MIN_ALERT_INTERVAL 1.5

USING_NS_AX;

namespace opendw
{

GameGui* GameGui::createWithZone(WorldZone* zone)
{
    CREATE_INIT(GameGui, initWithZone, zone);
}

static SpriteButton* createTopHudButton(const std::string& name, bool chop, float offsetY, const ax::Color3B& color)
{
    auto background = chop ? "hud/bubble-top-chop" : "hud/bubble-top";
    auto foreground = std::format("hud/bubble-icon-{}", name);
    auto button     = SpriteButton::createWithBackground(background, foreground, true);
    button->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
    button->setOpacity(HUD_BUTTON_OPACITY);
    button->setScale(GUI_SCALE * HUD_BUTTON_SCALE);
    auto sprite = button->getForegroundSprite();
    sprite->setPosition(sprite->getPosition() + Vec2::UNIT_Y * offsetY);
    sprite->setColor(color);
    return button;
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

    // Create hud node
    _hudNode = Node::create();
    addChild(_hudNode, 1);

    // Create hud labels
    _nameLabel = Label::createWithBMFont("console-shadow.fnt", "Player Name");
    _nameLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    _nameLabel->setScale(GUI_SCALE);
    _nameLabel->setOpacity(212);
    _hudNode->addChild(_nameLabel);
    _zoneLabel = Label::createWithBMFont("console-shadow.fnt", "Mystery Zone");
    _zoneLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    _zoneLabel->setScale(GUI_SCALE * 0.75F);
    _zoneLabel->setOpacity(212);
    _hudNode->addChild(_zoneLabel);
    _positionLabel = MultiLabel::createWithBMFont("console-shadow.fnt", "0 central :up: 0");
    _positionLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    _positionLabel->setScale(_zoneLabel->getScale() * 0.85F);
    _positionLabel->setOpacity(_zoneLabel->getOpacity() * 0.8F);
    _hudNode->addChild(_positionLabel);

    // 0x100059A15: Create health bar
    _healthBar = IconBar::createWithIcon("hud/heart", 5.0F);
    _healthBar->setIconColor(color_util::hexToColor("FF4A34"));
    _healthBar->setPadding(1.0F);
    _healthBar->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    _healthBar->setScale(GUI_SCALE * 0.4F);
    _hudNode->addChild(_healthBar);

    // 0x100059BCC: Create steam label
    _steamLabel = MultiLabel::createWithBMFont("console-shadow.fnt", ":gauge:");
    _steamLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    _steamLabel->setCascadeColorEnabled(true);  // setColorEmoji
    _hudNode->addChild(_steamLabel);

    // 0x10005A079: Create death label
    _deathLabel = Label::createWithBMFont("console.fnt", "You have died.");
    _deathLabel->setPosition(_director->getWinSize() * 0.5F);
    _deathLabel->setOpacity(212);
    _deathLabel->setVisible(false);
    _hudNode->addChild(_deathLabel);

    // Create hud buttons node
    _hudButtonsNode = Node::create();
    addChild(_hudButtonsNode, 8);

    // TODO: temp
    auto defaultCallback = [=]() { showAlert("Sorry, this feature isn't quite ready yet."); };

    // 0x10005A352: Create profile button
    _profileButton = SpriteButton::createWithSpriteFrame("hud/bubble-corner");
    _profileButton->setCallback(defaultCallback);
    _profileButton->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    _profileButton->setFlippedY(true);
    _profileButton->setScale(GUI_SCALE * HUD_BUTTON_SCALE);
    addChild(_profileButton);

    // 0x10005A44E: Create avatar picture
    _avatarPicture = new EntityAnimatedAvatar();
    _avatarPicture->initWithConfig(GameConfig::getMain()->getEntityForCode(0), "", ValueMapNull);
    _avatarPicture->autorelease();
    _avatarPicture->showHeadOnly();
    _avatarPicture->setScale(1.8F);
    _avatarPicture->setPosition(_profileButton->getContentSize().width * 0.425F,
                                math_util::getScaledHeight(_avatarPicture) * -0.4F);
    _profileButton->addChild(_avatarPicture);

    // 0x10005AAA4: Create shop button
    _shopButton = createTopHudButton("shop", true, 30.0F, Color3B::WHITE);
    _shopButton->setCallback(defaultCallback);
    _hudButtonsNode->addChild(_shopButton);
    _crownsLabel = Label::createWithBMFont("console-shadow.fnt", "0");
    _crownsLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
    _crownsLabel->setColor(color_util::hexToColor("FFFFA0"));
    _crownsLabel->setPosition(_shopButton->getForegroundSprite()->getPosition() - Vec2::UNIT_Y * 48.0F);
    _crownsLabel->setScale(1.25F);
    _shopButton->addChild(_crownsLabel);

    // 0x10005AE56: Create world button
    _worldButton = createTopHudButton("world", true, 20.0F, color_util::hexToColor("C1B09D"));
    _worldButton->setCallback([=]() { showTeleportInterface(nullptr); });
    _hudButtonsNode->addChild(_worldButton);

    // 0x10005AC33: Create social button
    _socialButton = createTopHudButton("social", true, 30.0F, color_util::hexToColor("C1B09D"));
    _socialButton->setCallback(defaultCallback);
    _hudButtonsNode->addChild(_socialButton);
    _socialLabel = Label::createWithBMFont("console-shadow.fnt", "1", TextHAlignment::CENTER);
    _socialLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
    _socialLabel->setPosition(_socialButton->getForegroundSprite()->getPosition() - Vec2::UNIT_Y * 48.0F);
    _socialLabel->setScale(1.25F);
    _socialButton->addChild(_socialLabel);

    // 0x10005A980: Create crafting button
    _craftingButton = createTopHudButton("crafting", true, 20.0F, color_util::hexToColor("C1B09D"));
    _craftingButton->setCallback(defaultCallback);
    _hudButtonsNode->addChild(_craftingButton);

    // 0x10005A843: Create inventory button
    _inventoryButton = createTopHudButton("inventory", false, 20.0F, color_util::hexToColor("C1B09D"));
    _inventoryButton->setCallback(defaultCallback);
    _hudButtonsNode->addChild(_inventoryButton);

    // 0x10005AF75: Create map button
    _mapButton = SpriteButton::createWithSpriteFrame("hud/map");
    _mapButton->setCallback(defaultCallback);
    _mapButton->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    _mapButton->setOpacity(HUD_BUTTON_OPACITY);
    _mapButton->setScale(GUI_SCALE);
    _hudButtonsNode->addChild(_mapButton);

    // 0x10005B057: Create console button
    _consoleButton = SpriteButton::createWithBackground("hud/bubble-corner", "hud/bubble-icon-chat");
    _consoleButton->setCallback(defaultCallback);
    _consoleButton->setColor(color_util::hexToColor("E0CCAD"));
    _consoleButton->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _consoleButton->setOpacity(HUD_BUTTON_OPACITY);
    _consoleButton->setScale(GUI_SCALE * HUD_BUTTON_SCALE);
    _consoleButton->getForegroundSprite()->setScale(0.8F);
    _consoleButton->getForegroundSprite()->setPositionX(_consoleButton->getForegroundSprite()->getPositionX() - 20.0F);
    _consoleButton->getForegroundSprite()->setColor(_consoleButton->getColor());
    _hudButtonsNode->addChild(_consoleButton);

    AXLOGI("[GameGui] Initialized");
    sMain = this;
    return true;
}

void GameGui::onEnter()
{
    Node::onEnter();
#ifdef AX_PLATFORM_PC
    addEventListener(RenderViewImpl::EVENT_WINDOW_RESIZED, AX_CALLBACK_0(GameGui::onWindowResized, this));
#endif
    addEventListener(events::kNotifyAlert, EVENT_CALLBACK_REF(Value*, showAlert));
    addEventListener(events::kNotifyBigAlert, EVENT_CALLBACK_REF(Value*, showBigAlert));
    addEventListener(events::kPlayerAppearanceChanged, EVENT_CALLBACK_REF(ValueMap*, onPlayerAppearanceChanged));
    addEventListener(events::kPlayerHealthChanged, EVENT_CALLBACK_EX(float*, onHealthChanged, data[1], data[2]));
    addEventListener(events::kSteamChanged, EVENT_CALLBACK_REF(float*, onSteamChanged));
    addEventListener(events::kSteamCooldownBegan, AX_CALLBACK_0(GameGui::onSteamCooldownBegan, this));
    addEventListener(events::kSteamCooldownEnded, AX_CALLBACK_0(GameGui::onSteamCooldownEnded, this));
    addEventListener(events::kDeathMessageChanged, EVENT_CALLBACK_REF(std::string*, onDeathMessageChanged));
    addEventListener(events::kPlayerEntered, AX_CALLBACK_0(GameGui::onPlayerCountChanged, this));
    addEventListener(events::kPlayerExited, AX_CALLBACK_0(GameGui::onPlayerCountChanged, this));
    onWindowResized();
}

void GameGui::onExit()
{
    removeEventListeners();
    Node::onExit();
}

void GameGui::update(float deltaTime)
{
    Node::update(deltaTime);

    // 0x10005D0B2: Show next pending alert if it is time
    if (!_pendingAlerts.empty() && utils::gettime() > _lastAlertShownAt + MIN_ALERT_INTERVAL)
    {
        auto it = _pendingAlerts.begin();
        showAlert(*it);
        _pendingAlerts.erase(it);
    }

    // TODO: use events
    _nameLabel->setString(Player::getMain()->getUsername());
    _zoneLabel->setString(_zone->getZoneName());
    _positionLabel->setString(getPositionDescription());
    _healthBar->setPosition(_nameLabel->getPositionX() + math_util::getScaledWidth(_nameLabel) + 10.0F, _nameLabel->getPositionY());
    _steamLabel->setPosition(_healthBar->getPosition() + Vec2(math_util::getScaledWidth(_healthBar) + 15.0F, -2.0F));
}

void GameGui::ready()
{
    // TODO: implement
}

void GameGui::clear()
{
    _pendingAlerts.clear();
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

void GameGui::setHudVisible(bool visible)
{
    _hudNode->setVisible(visible);
    _hudButtonsNode->setVisible(visible);
    _profileButton->setVisible(visible);
}

void GameGui::showTeleportInterface(BaseBlock* block)
{
    // TODO: finish
    setHudVisible(false);
    _teleportPanel->showFromBlock(block);
    _eventDispatcher->dispatchCustomEvent(events::kZoneTeleportActivated);
}

void GameGui::hideTeleportInterface()
{
    // TODO: finish
    setHudVisible(true);
    _teleportPanel->setVisible(false);
    _eventDispatcher->dispatchCustomEvent(events::kZoneTeleportDeactivated);
}

void GameGui::showTeleportZones(const std::string& type, const std::vector<ZoneSearchInfo>& data)
{
    _teleportPanel->showZones(type, data);
}

void GameGui::showAlert(const std::string& text)
{
    // Add to pending alerts if not enough time has passed since the previous alert
    if (utils::gettime() <= _lastAlertShownAt + MIN_ALERT_INTERVAL)
    {
        if (text != _lastAlertShown)
        {
            _pendingAlerts.push_back(text);
        }

        return;
    }

    // Create and display alert label
    auto label = MultiLabel::createWithBMFont("console-shadow.fnt", text);
    label->setMaxLineWidth(label->getBMFontSize() * 50.0F);  // 30 for small screen
    auto& winSize = _director->getWinSize();
    label->setPosition(winSize.width * 0.5F, winSize.height * 0.4F);
    label->setOpacity(0);
    auto duration = text.length() > 50 ? 7.0F : 4.0F;
    auto moveUp   = MoveBy::create(duration, Vec2::UNIT_Y * 40.0F);
    auto fadeIn   = FadeIn::create(duration * 0.333F);
    auto delay    = DelayTime::create(duration * 0.333F);
    auto fadeOut  = FadeOut::create(duration * 0.333F);
    auto cleanup  = CallFuncN::create(&Node::removeFromParent);
    auto sequence = Sequence::create({fadeIn, delay, fadeOut, cleanup});
    label->runAction(Spawn::createWithTwoActions(moveUp, sequence));
    addChild(label, 9);
    AudioManager::getInstance()->playSfx("notification", 1.0F, 0.21F);
    _lastAlertShown   = text;
    _lastAlertShownAt = utils::gettime();
}

void GameGui::showAlert(const Value& data)
{
    // TODO: finish
    if (data.getType() == Value::Type::STRING)
    {
        showAlert(data.asString());
    }
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

void GameGui::showBigAlert(const Value& data)
{
    if (data.getType() == Value::Type::MAP)
    {
        auto& map     = data.asValueMap();
        auto title    = map_util::getString(map, "t");
        auto subtitle = map_util::getString(map, "t2");
        showBigAlert(title, subtitle);
    }
    else
    {
        // Assume string
        showBigAlert(data.asString());
    }
}

const char* GameGui::getRespawnMessage() const
{
#ifdef AX_PLATFORM_PC
    return "Press space bar to respawn.";
#else
    return "Tap the screen to respawn.";
#endif
}

std::string GameGui::getPositionDescription() const
{
    auto player    = Player::getMain();
    auto position  = player->getBlockPosition();
    auto x         = (int16_t)(_zone->getBlocksWidth() * -0.5F + position.x);
    auto longitude = x == 0 ? "center" : x > 0 ? "east" : "west";
    auto depth     = (int16_t)(position.y - (_zone->getBiomeType() == Biome::DEEP ? -1000.0F : 200.0F));
    auto test      = depth > 0 ? ":down:" : ":up:";
    return std::format("{} {} {}{}m", abs(x), longitude, test, abs(depth));
}

void GameGui::onHealthChanged(float health, float maxHealth)
{
    _healthBar->setMaxValue(maxHealth);
    _healthBar->setValue(health);
    _deathLabel->setVisible(health <= 0.0F);
}

void GameGui::onSteamChanged(float steam)
{
    auto ceiled = (int)ceilf(steam);
    _steamLabel->setString(std::format("{}:gauge:", ceiled));
}

void GameGui::onSteamCooldownBegan()
{
    _steamLabel->setColor(Color3B::RED);
}

void GameGui::onSteamCooldownEnded()
{
    _steamLabel->setColor(Color3B::WHITE);
}

void GameGui::onDeathMessageChanged(const std::string& deathMessage)
{
    _deathLabel->setString(deathMessage);
}

void GameGui::onPlayerCountChanged()
{
    auto count = _zone->getPeerCount();
    _socialLabel->setString(std::to_string(count + 1));
}

void GameGui::onPlayerAppearanceChanged(const ValueMap& data)
{
    _avatarPicture->updateAppearance(data);
    _avatarPicture->showHeadOnly();
}

void GameGui::onWindowResized()
{
    auto viewOffset  = _director->getVisibleOrigin();
    auto visibleSize = _director->getVisibleSize();
    auto left        = floorf(viewOffset.x);
    auto bottom      = floorf(viewOffset.y);
    auto right       = ceilf(visibleSize.width + viewOffset.x);
    auto top         = ceilf(visibleSize.height + viewOffset.y);
    auto smallScreen = visibleSize.width <= 1024.0F;

    // Update elements
    _profileButton->setPosition(left, top);
    _nameLabel->setPosition(_profileButton->getPositionX() + math_util::getScaledWidth(_profileButton) + PANEL_MARGIN, top - PANEL_MARGIN);
    _zoneLabel->setPosition(_nameLabel->getPositionX(), _nameLabel->getPositionY() - math_util::getScaledHeight(_nameLabel) - 5.0F);
    _positionLabel->setPosition(_zoneLabel->getPositionX(), _zoneLabel->getPositionY() - math_util::getScaledHeight(_zoneLabel) - 5.0F);
    auto buttonWidth  = math_util::getScaledWidth(_shopButton);  // Should be the same for all buttons
    auto buttonOffset = buttonWidth - buttonWidth * 0.15F;       // Chop
    _shopButton->setPosition(right - 100.0F, top);
    _worldButton->setPosition(_shopButton->getPositionX() - buttonOffset, top);
    _socialButton->setPosition(_worldButton->getPositionX() - buttonOffset, top);
    _craftingButton->setPosition(_socialButton->getPositionX() - buttonOffset, top);
    _inventoryButton->setPosition(_craftingButton->getPositionX() - buttonOffset, top);
    _mapButton->setPosition(right, bottom);
    _consoleButton->setPosition(left, bottom);
}

}  // namespace opendw

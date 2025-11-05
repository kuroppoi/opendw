#include "TeleportIcon.h"

#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"
#include "AudioManager.h"
#include "CommonDefs.h"
#include "GameConfig.h"
#include "GameManager.h"
#include "Player.h"

#define MAX_PEER_ICONS 9

USING_NS_AX;

namespace opendw
{

TeleportIcon* TeleportIcon::createWithInfo(const ZoneSearchInfo& info)
{
    CREATE_INIT(TeleportIcon, initWithInfo, info);
}

bool TeleportIcon::initWithInfo(const ZoneSearchInfo& info)
{
    if (!Node::init())
    {
        return false;
    }

    auto& scenario    = info.scenario;
    auto showScenario = !scenario.empty();
    _target           = info.documentId;
    _premium          = info.accessibility == "p";
    _inaccessible     = info.accessibility == "i";
    setEnabled(true);
    setCascadeOpacityEnabled(true);

    // 0x100100CA0: Create biome/scenario icon
    auto config       = GameManager::getInstance()->getConfig();
    auto& biomeConfig = config->getBiomeConfig(info.biome);
    auto icon         = map_util::getString(biomeConfig, "icon", "icons/mountains");

    if (!scenario.empty())
    {
        auto& value = map_util::getValue(config->getData(), std::format("scenarios.{}.icon", scenario));

        if (!value.isNull())
        {
            icon         = value.asString();
            showScenario = false;  // Icon present; no longer need to show it as text
        }
    }

    auto iconSprite = Sprite::createWithSpriteFrameName(icon);
    iconSprite->setScale(0.7F);
    iconSprite->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    iconSprite->setColor(_inaccessible ? color_util::hexToColor("FFDCDC") : Color3B::WHITE);
    addChild(iconSprite);

    // 0x100100D71: Create marker icon
    auto marker = Sprite::createWithSpriteFrameName(_premium ? "icons/marker" : "icons/marker-free");
    marker->setScale(0.7F);
    marker->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    marker->setPositionY(20.0F);
    addChild(marker);

    // 0x100100DF5: Create name label
    auto nameLabel = Label::createWithBMFont("console.fnt", info.name);
    nameLabel->setScale(0.75F);
    nameLabel->setPositionY(-13.0F);
    nameLabel->setColor(
        color_util::lerpColor(color_util::hexToColor("352A0A"), Color3B::RED, _inaccessible ? 0.234F : 0.0F));
    addChild(nameLabel);

    // 0x100100F8E: Create status label
    // TODO: track player's premium status
    auto status =
        std::format("{}% explored{}", info.explored, info.status.empty() ? "" : std::format(", {}", info.status));
    auto statusLabel = Label::createWithBMFont("console.fnt", _inaccessible ? "UNLOCK" : status);
    statusLabel->setScale(0.6F);
    statusLabel->setPositionY(-29.0F);
    statusLabel->setColor(
        color_util::lerpColor(color_util::hexToColor("503C2B"), Color3B::RED, _inaccessible ? 0.234F : 0.0F));
    addChild(statusLabel);

    // 0x100101069: Create scenario label if necessary
    if (showScenario)
    {
        auto scenarioLabel = Label::createWithBMFont("console.fnt", scenario);
        scenarioLabel->setScale(0.67F);
        scenarioLabel->setPositionY(statusLabel->getPositionY() - math_util::getScaledHeight(statusLabel) - 3.0F);
        scenarioLabel->setColor(color_util::lerpColor(statusLabel->getColor(), Color3B::RED, 0.234F));
        addChild(scenarioLabel);
    }

    // 0x100101121: Show protection lock if world is protected
    if (info.protection > 0)
    {
        auto lockSprite = Sprite::createWithSpriteFrameName("map/locked");
        lockSprite->setScale(0.5F);
        lockSprite->setPosition(-35.0F, 40.0F);
        lockSprite->setColor(color_util::hexToColor("D4D4D4"));
        addChild(lockSprite);
    }

    // 0x1001011A2: Create player icons
    auto iconCount = MIN(MAX_PEER_ICONS, info.playerCount);

    for (auto i = 0; i < iconCount; i++)
    {
        auto playerSprite = Sprite::createWithSpriteFrameName("icons/player+hd");
        playerSprite->setScale(0.2F * 0.9F);
        auto index = i + (i > 1 && iconCount < 5);
        playerSprite->setPosition(index * 8.0F + 43.0F, (index % 3) * -22.0F + 60.0F);
        playerSprite->setColor(i < info.followeeCount ? color_util::hexToColor("FFE930")    // Followee color
                                                      : color_util::hexToColor("BDA57F"));  // Normal color
        addChild(playerSprite);
    }

    return true;
}

void TeleportIcon::activate()
{
    // TODO: check player's premium status
    // TODO: handle inaccessibility
    AudioManager::getInstance()->playButtonSfx();
    Player::getMain()->teleportToZone(_target);
}

void TeleportIcon::setHighlighted(bool highlighted)
{
    stopAllActions();
    setOpacity(0xFF);
    runAction(ScaleTo::create(0.12F, highlighted ? 1.2F : 1.0F));
}

void TeleportIcon::moveTo(const Point& point)
{
    auto offset   = random(20.0F, 30.0F);
    auto duration = random(1.8F, 2.4F);
    auto fadeTo   = FadeTo::create(duration * 0.25F, 0xFF);  // NOTE: originally duration * 0.5
    auto moveTo   = EaseIn::create(MoveTo::create(duration, point), 0.4F);
    setOpacity(0);
    setPosition(point.x + offset, point.y);
    runAction(Spawn::createWithTwoActions(fadeTo, moveTo));
}

}  // namespace opendw

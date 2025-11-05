#include "TeleportPanel.h"

#include "gui/GameGui.h"
#include "gui/SpriteButton.h"
#include "gui/TeleportIcon.h"
#include "input/InputManager.h"
#include "network/tcp/MessageIdent.h"
#include "util/AxUtil.h"
#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"
#include "zone/BaseBlock.h"
#include "GameConfig.h"
#include "GameManager.h"

#define MAX_HIGHLIGHT_DISTANCE 75.0F  // Originally 50

USING_NS_AX;

namespace opendw
{

bool TeleportPanel::init()
{
    if (!Panel::initWithStyle("v2-transparent/brass"))
    {
        return false;
    }

    // TODO: might not scale well
    auto& winSize = _director->getWinSize();
    setAnchorPoint(Point::ANCHOR_MIDDLE);
    setPosition(winSize * 0.5F);
    setContentSize(winSize - Size(200.0F, 50.0F));
    setBackgroundTexture("map-background.png", 0xD4);

    // 0x1000FDB78: Create teleporter node
    _teleportersNode = Node::create();
    addChild(_teleportersNode, 1);

    // 0x1000FDBC0: Create group button node
    _groupNode = Node::create();
    _groupNode->setPosition(_contentSize - Vec2(40.0F, 40.0F));
    _groupNode->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
    addChild(_groupNode, 1);

    // Create group indicator
    _groupIndicator = Sprite::createWithSpriteFrameName("icons/triangle-up");
    _groupIndicator->setRotation(-90.0F);
    _groupIndicator->setScale(0.4F);
    _groupIndicator->setColor(color_util::hexToColor("393224"));
    addChild(_groupIndicator, 1);
    return true;
}

void TeleportPanel::onEnter()
{
    Panel::onEnter();
    schedule(AX_CALLBACK_1(TeleportPanel::update, this), 0.05F, "update");
}

void TeleportPanel::onExit()
{
    unschedule("update");
    Panel::onExit();
}

void TeleportPanel::update(float deltaTime)
{
    Panel::update(deltaTime);

    if (!ax_util::isNodeVisible(this))
    {
        return;
    }

    // 0x1000FFDFC: Highlight hovered teleporter
#if defined(AX_PLATFORM_PC)
    auto hovered = findCursorHoveredTeleport();

    if (hovered != _highlightedTeleport)
    {
        if (_highlightedTeleport)
        {
            _highlightedTeleport->setHighlighted(false);
        }

        if (hovered)
        {
            hovered->setHighlighted(true);
        }

        _highlightedTeleport = hovered;
    }
#endif
}

void TeleportPanel::showFromBlock(BaseBlock* block)
{
    _origin = block;
    _groupIndicator->setOpacity(0);
    _currentGroup = "";
    showGroups(ValueVectorNull);
    clearTeleporterInterface(false);
    setVisible(true);
    // TODO: have to request recent if player has more than 2 recently visited zones
    GameManager::getInstance()->sendMessage(MessageIdent::ZONE_SEARCH, "Popular");
}

void TeleportPanel::clearGroupInterface()
{
    _groupNode->removeAllChildren();
    _groupButtons.clear();
}

void TeleportPanel::clearTeleporterInterface(bool fancy)
{
    _highlightedTeleport = nullptr;

    if (!fancy)
    {
        if (_noneFoundLabel)
        {
            _noneFoundLabel->removeFromParent();
            _noneFoundLabel = nullptr;
        }

        _teleportersNode->removeAllChildren();
        return;
    }

    // 0x100100331: Fade out label
    if (_noneFoundLabel)
    {
        ax_util::fadeOutAndRemove(_noneFoundLabel);
        _noneFoundLabel = nullptr;
    }

    // 0x100100371: Fade out icons
    for (auto& child : _teleportersNode->getChildren())
    {
        auto icon = static_cast<TeleportIcon*>(child);

        // Prevent duplicate action running
        if (!icon->isEnabled())
        {
            continue;
        }

        auto duration = random(1.0F, 1.2F);
        auto fadeTo   = FadeTo::create(duration, 0);
        auto moveBy   = MoveBy::create(duration, Vec2::UNIT_X * -150.0F);
        auto easeIn   = EaseIn::create(moveBy, 0.4F);
        auto delay    = DelayTime::create(duration);
        auto callFunc = CallFunc::create([=]() { child->removeFromParent(); });
        auto sequence = Sequence::create({delay, callFunc});
        auto spawn    = Spawn::create({fadeTo, easeIn, sequence});
        icon->runAction(spawn);
        icon->setEnabled(false);
    }
}

void TeleportPanel::showGroups(const ValueVector& groups)
{
    ValueVector copy = groups;  // Explicit copy
    clearGroupInterface();

    // 0x1000FDE8E: Set to root group if null
    if (copy == ValueVectorNull)
    {
        auto config = GameManager::getInstance()->getConfig();
        copy        = map_util::getArray(config->getData(), "zone search v2");
    }
    else
    {
        copy.push_back(Value::Null);  // Back button
    }

    auto currentY = 0.0F;

    // 0x1000FDF8F: Create group buttons
    for (auto& group : copy)
    {
        auto button = createGroupButton(group);
        button->setPositionY(currentY);
        currentY -= (math_util::getScaledHeight(button) + 16.0F);
    }

    // 0x1000FE0AE: Create exit button
    auto exitButton = SpriteButton::createWithSpriteFrame("buttons/brass");
    exitButton->setTitle("Exit");
    exitButton->setTitleColor(color_util::hexToColor("FFF032"));
    exitButton->setColor(color_util::hexToColor("96320A"));
    exitButton->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
    exitButton->setPositionY(currentY);
    exitButton->setCallback([=]() { GameGui::getMain()->hideTeleportInterface(); });
    _groupNode->addChild(exitButton);

    // NOTE: does a bunch of weird stuff after this to align the buttons
    // but we're going to conveniently ignore that for now...
}

SpriteButton* TeleportPanel::createGroupButton(const Value& group)
{
    std::string key = "Back";
    auto color      = color_util::hexToColor("FAC80A");

    // 0x1000FE6C1: Determine title & color
    if (!group.isNull())
    {
        if (group.getType() == Value::Type::STRING)
        {
            key   = group.asString();
            color = Color3B::WHITE;
        }
        else
        {
            key = (*group.asValueMap().begin()).first;
        }
    }

    // 0x1000FE751: Create button
    auto button = SpriteButton::createWithSpriteFrame("buttons/brass");
    button->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
    button->setTitle(key);
    button->setTitleColor(color_util::hexToColor("FFF032"));
    button->setColor(color);
    button->setCallback([=]() { onGroupSelected(group); });
    _groupNode->addChild(button);
    _groupButtons.insert(key, button);
    return button;
}

void TeleportPanel::onGroupSelected(const Value& group)
{
    Value copy = group;  // Explicit copy

    if (copy.getType() != Value::Type::STRING)
    {
        _groupIndicator->runAction(FadeTo::create(0.5F, 0));
        _currentGroup = "";

        if (copy.isNull())
        {
            // Return to root group
            showGroups(ValueVectorNull);
            return;
        }

        auto& map = copy.asValueMap();
        auto it   = map.begin();
        showGroups((*it).second.asValueVector());
        return;
    }

    auto key = group.asString();

    if (key == "Search")
    {
        // TODO: implement search feature
        return;
    }

    // Show group button spinner
    auto it = _groupButtons.find(key);

    if (it != _groupButtons.end())
    {
        (*it).second->showSpinner();
    }

    // Send zone search request with group
    clearTeleporterInterface(true);
    GameManager::getInstance()->sendMessage(MessageIdent::ZONE_SEARCH, key);
}

void TeleportPanel::showZones(const std::string& type, const std::vector<ZoneSearchInfo>& data)
{
    clearTeleporterInterface(true);

    // 0x1000FF648: Create zone icons
    size_t index = 0;
    auto center  = _contentSize * Vec2(0.4F, 0.5F);

    for (auto& info : data)
    {
        auto icon = TeleportIcon::createWithInfo(info);
        _teleportersNode->addChild(icon);

        if (index > 0)
        {
            // NOTE: Original does a lot more math
            auto angle  = (float)(index - 1) / (data.size() - 1) * 360.0F;
            auto offset = math_util::rotateVector(Vec2(1.0F, -1.0F), angle);
            offset.x *= _contentSize.width * 0.175F;
            offset.y *= _contentSize.height * 0.2375F;
            icon->moveTo(center + offset);
        }
        else
        {
            icon->moveTo(center);
        }

        index++;
    }

    auto it = _groupButtons.find(type);

    if (it != _groupButtons.end())
    {
        auto button = (*it).second;
        button->hideSpinner();

        // 0x1000FF243: Move group indicator
        if (type != _currentGroup)
        {
            auto position = _groupNode->getPosition() + button->getPosition();
            auto size     = math_util::getScaledSize(button);
            _groupIndicator->setPosition(position.x - size.width - 30.0F, position.y - size.height * 0.5F);
            _groupIndicator->setOpacity(0);
            _groupIndicator->runAction(FadeIn::create(0.5F));
        }
    }

    // 0x1000FF9AF: Show "no worlds found" label if no worlds were found
    if (data.empty())
    {
        _noneFoundLabel = Label::createWithBMFont("console.fnt", "No worlds found.");
        _noneFoundLabel->setColor(Color3B::BLACK);
        _noneFoundLabel->setOpacity(0);
        _noneFoundLabel->setPosition(center);
        _noneFoundLabel->runAction(FadeIn::create(0.5F));
        addChild(_noneFoundLabel);
    }

    _currentGroup = type;  // NOTE: not set in v2
}

TeleportIcon* TeleportPanel::findClosestTeleportToPoint(const Point& point)
{
    TeleportIcon* result = nullptr;
    auto closest         = 0.0F;
    auto nodePoint       = _teleportersNode->convertToNodeSpace(point);

    for (auto& child : _teleportersNode->getChildren())
    {
        auto teleport = static_cast<TeleportIcon*>(child);

        // Skip if not enabled
        if (!teleport->isEnabled())
        {
            continue;
        }

        auto position = teleport->getPosition() + Vec2(20.0F, 20.0F);  // HACK: Slightly offset origin
        auto distance = hypotf(position.x - nodePoint.x, position.y - nodePoint.y);

        if (distance <= MAX_HIGHLIGHT_DISTANCE && (!result || distance < closest))
        {
            result  = teleport;
            closest = distance;
        }
    }

    return result;
}

TeleportIcon* TeleportPanel::findCursorHoveredTeleport()
{
#if defined(AX_PLATFORM_PC)
    auto inputManager    = GameManager::getInstance()->getInputManager();
    auto& cursorPosition = inputManager->getCursorPosition();
    return findClosestTeleportToPoint(cursorPosition);
#else
    return nullptr;
#endif
}

bool TeleportPanel::onPointerDown(Touch* touch)
{
#if defined(AX_PLATFORM_PC)
    if (_highlightedTeleport)
    {
        _highlightedTeleport->activate();
        return true;
    }
#else
    auto teleport = findClosestTeleportToPoint(touch->getLocation());

    if (teleport)
    {
        teleport->activate();
        return true;
    }
#endif

    return false;
}

}  // namespace opendw

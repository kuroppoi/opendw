#include "GameMap.h"

#include "base/GameConfig.h"
#include "base/Item.h"
#include "base/ItemCodes.h"
#include "base/Player.h"
#include "event/EventNames.h"
#include "gui/widget/MultiLabel.h"
#include "gui/widget/Panel.h"
#include "gui/widget/SpriteButton.h"
#include "gui/GameGui.h"
#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"
#include "zone/MetaBlock.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"

#define MIN_MAP_WIDTH     800.0F
#define INFO_PANEL_WIDTH  105.0F
#define TOGGLE_ACTION_TAG 0x1  // Tag for show/dismissal animation actions

USING_NS_AX;

namespace opendw
{

static constexpr uint32_t kDefaultColors[] = {0xFFFFFF, 0xF1F1ED, 0xE4E3DB, 0xD7D5C9, 0xCAC7B8, 0xB8B5A0, 0xA7A389};
static constexpr float kDefaultDepths[]    = {0.03F, 0.05F, 0.08F, 0.12F, 0.17F, 0.26F, 0.3F};
static constexpr auto kButtonColor         = 0xD68901;  // Button foreground RGB

GameMap::~GameMap()
{
    AX_SAFE_RELEASE(_surfaceMap.texture);
    AX_SAFE_DELETE_ARRAY(_surfaceMap.pixels);
    AX_SAFE_RELEASE(_chunkMap.texture);
    AX_SAFE_DELETE_ARRAY(_chunkMap.pixels);
}

GameMap* GameMap::createWithZone(WorldZone* zone)
{
    CREATE_INIT(GameMap, initWithZone, zone);
}

bool GameMap::initWithZone(WorldZone* zone)
{
    if (!Node::init())
    {
        return false;
    }

    _zone = zone;
    setCascadeOpacityEnabled(true);
    setVisible(false);  // Inactive by default
    setOpacity(0);
    setScale(0.888F);

    // Init surface map
    _surfaceMap.texture = new Texture2D();
    _surfaceMap.texture->autorelease();
    _surfaceMap.texture->retain();
    _surfaceMap.size  = -1;
    _surfaceMapSprite = Sprite::createWithTexture(_surfaceMap.texture);
    _surfaceMapSprite->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
    _surfaceMapSprite->setAnchorPoint(Point::ZERO);
    addChild(_surfaceMapSprite, 8);

    // Init chunk map
    _chunkMap.texture = new Texture2D();
    _chunkMap.texture->autorelease();
    _chunkMap.texture->retain();
    _chunkMap.size  = -1;
    _chunkMapSprite = Sprite::createWithTexture(_chunkMap.texture);
    _chunkMapSprite->setAnchorPoint(Point::ZERO);
    addChild(_chunkMapSprite, 9);

    // Create panel
    _panel = Panel::createWithStyle("v2-transparent/brass");
    _panel->setBackgroundTexture("map-background.png", 128);
    _panel->setPosition(-10.0F, -10.0F);
    addChild(_panel, 11);

    // Create info panel
    _infoPanel = Panel::createWithStyle("v2-opaquer/brass");
    _infoPanel->setBorderScale(0.65F);
    _infoPanel->setChop(Panel::Edge::LEFT);
    _infoPanel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    addChild(_infoPanel, 10);

    // Create title label
    _titleLabel = Label::createWithBMFont("menu.fnt", " ");
    _titleLabel->setScale(0.4F);
    _titleLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
    addChild(_titleLabel, 13);

    // Create player icon
    _playerIcon = Sprite::createWithSpriteFrameName("map/person");
    _playerIcon->setScale(0.75F);
    _playerIcon->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    addChild(_playerIcon, 14);

    // Create meta map
    _metaMap = Node::create();
    _metaMap->setCascadeOpacityEnabled(true);
    addChild(_metaMap, 12);

    // Create events map
    _eventsMap = Node::create();
    _eventsMap->setCascadeOpacityEnabled(true);
    addChild(_eventsMap, 13);

    // Create vector layer node
    _vectorNode = DrawNode::create();
    addChild(_vectorNode, 12);

    // Create bookmark button
    auto buttonColor = color_util::rgbToColor(kButtonColor);
    _bookmarkButton  = SpriteButton::createWithBackground("buttons/brass", "hud/icon-star");
    _bookmarkButton->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _bookmarkButton->getForegroundSprite()->setColor(buttonColor);
    _bookmarkButton->setCallback([=]() {
        _zone->toggleBookmark();
        updateBookmark();
    });  // 0x100126D35
    _infoPanel->addChild(_bookmarkButton, 3);

    // Create snapshot button
    _snapshotButton = SpriteButton::createWithBackground("buttons/brass", "hud/icon-camera");
    _snapshotButton->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _snapshotButton->getForegroundSprite()->setColor(buttonColor);
    _snapshotButton->setCallback([=]() {
        // TODO: implementation relies on configurable dialog system
        GameGui::getMain()->showAlert("Sorry, this feature isn't quite ready yet.");
    });
    _infoPanel->addChild(_snapshotButton, 3);

    // Create rangefinder button
    _rangefinderButton = SpriteButton::createWithBackground("buttons/brass", "hud/icon-range");
    _rangefinderButton->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _rangefinderButton->getForegroundSprite()->setColor(buttonColor);
    _rangefinderButton->setCallback([=]() { GameGui::getMain()->toggleProtectorRangeVisibility(); });
    _infoPanel->addChild(_rangefinderButton, 3);

    return true;
}

void GameMap::onEnter()
{
    Node::onEnter();
    addEventListener(events::kChunkExplored, EVENT_CALLBACK_REF(int32_t*, onChunkExplored));
    addEventListener(events::kMetaBlockChanged, EVENT_CALLBACK_REF(int32_t*, onMetaBlockChanged));
    addEventListener(events::kPlayerSkillChanged, AX_CALLBACK_0(GameMap::adjustForSkill, this));
    addEventListener(events::kPlayerAccessoriesChanged,
                     AX_CALLBACK_0(GameMap::adjustForSkill, this));  // Treat as if skill have changed
    addEventListener(events::kPlayerDeathEvent, AX_CALLBACK_0(GameMap::onPlayerDeath, this));
    addEventListener(events::kZoneStatusChanged, AX_CALLBACK_0(GameMap::onZoneStatusChanged, this));
}

void GameMap::onExit()
{
    removeEventListeners();
    Node::onExit();
}

void GameMap::ready()
{
    // Update title label
    std::string title = _zone->getZoneName();
    std::transform(title.begin(), title.end(), title.begin(), ::toupper);
    _titleLabel->setString(title);

    // Load ground colors from biome config
    auto& data       = map_util::getArray(_zone->getBiomeConfig(), "map.ground");
    ssize_t count    = data == ValueVectorNull ? 7 : data.size();
    uint32_t* colors = new uint32_t[count];
    float* depths    = new float[count];

    if (data == ValueVectorNull)
    {
        std::copy(std::begin(kDefaultColors), std::end(kDefaultColors), colors);
        std::copy(std::begin(kDefaultDepths), std::end(kDefaultDepths), depths);
    }
    else
    {
        for (ssize_t i = 0; i < count; i++)
        {
            auto& array = data[i].asValueVector();
            colors[i]   = std::stoi(array[0].asString(), nullptr, 16);
            depths[i]   = array[1].asFloat();
        }
    }

    // Populate surface map texture
    auto width          = _zone->getBlocksWidth();
    auto height         = _zone->getBlocksHeight();
    auto surfaceTop     = _zone->getSurfaceTop();
    auto surfaceBottom  = _zone->getSurfaceBottom();
    auto surfaceAverage = (int16_t)floor(math_util::lerp(surfaceTop, surfaceBottom, 0.5F));
    auto textureSize    = (ssize_t)_zone->getBlocksWidth() * _zone->getBlocksWidth() * 4;

    if (_surfaceMap.size != textureSize)
    {
        // Realloc if size changed
        AX_SAFE_DELETE_ARRAY(_surfaceMap.pixels);
        _surfaceMap.pixels = new uint8_t[textureSize];
        _surfaceMap.size   = textureSize;
    }

    // Simulates the white-33-percent layer
    std::fill_n(reinterpret_cast<uint32_t*>(_surfaceMap.pixels), textureSize >> 2, 0x55FFFFFF);

    for (int16_t i = 0; i < width; i++)
    {
        auto surface     = _zone->getSurfaceAt(i);
        auto elevation   = surface - surfaceTop;
        auto layerHeight = 1.0F;
        auto currentY    = surface;

        for (ssize_t j = 0; j < count; j++)
        {
            auto color = colors[count == 1 || j > 0 || surface < surfaceAverage ? j : 1];
            auto depth = depths[j];
            auto scale = (float)elevation / height / count * (j < 2 ? 2.0F : 0.5F);
            layerHeight -= (depth - scale);
            auto end = j + 1 >= count ? height : height - (int)((height - surface) * layerHeight);

            for (; currentY < end; currentY++)
            {
                auto offset = ((ssize_t)currentY * width + i) * 4;
                AX_ASSERT(offset >= 0 && offset < textureSize);

                if (offset < 0 || offset >= textureSize)
                {
                    AXLOGW("[GameMap] Surface map offset out of bounds! {}/{} @ {}x{}", offset, textureSize, i, currentY);
                    continue;  // Shouldn't happen, but let's not take any chances.
                }

                _surfaceMap.pixels[offset++] = (color >> 16) & 0xFF;
                _surfaceMap.pixels[offset++] = (color >> 8) & 0xFF;
                _surfaceMap.pixels[offset++] = color & 0xFF;
                _surfaceMap.pixels[offset++] = 0xFF;
            }
        }
    }

    _surfaceMap.texture->initWithData(_surfaceMap.pixels, textureSize, backend::PixelFormat::RGBA8, width, height);
    _surfaceMap.texture->setAliasTexParameters();
    _surfaceMapSprite->setTextureRect({0.0F, 0.0F, (float)width, (float)height});
    AX_SAFE_DELETE_ARRAY(depths);
    AX_SAFE_DELETE_ARRAY(colors);

    // Populate explored chunk map texture
    auto chunkCount   = _zone->getChunkCount();
    auto chunksWidth  = _zone->getChunkCountX();
    auto chunksHeight = _zone->getChunkCountY();
    textureSize       = (ssize_t)chunkCount * 4;

    if (_chunkMap.size != textureSize)
    {
        // Realloc if size changed
        AX_SAFE_DELETE_ARRAY(_chunkMap.pixels);
        _chunkMap.pixels = new uint8_t[textureSize];
        _chunkMap.size   = textureSize;
    }

    memset(_chunkMap.pixels, 0, textureSize);

    for (int32_t i = 0; i < chunkCount; i++)
    {
        if (!_zone->isChunkExplored(i))
        {
            _chunkMap.pixels[i * 4 + 3] = 0x7F;  // Alpha byte
        }
    }

    _chunkMap.texture->initWithData(_chunkMap.pixels, textureSize, backend::PixelFormat::RGBA8, chunksWidth, chunksHeight);
    _chunkMap.texture->setAliasTexParameters();
    _chunkMapSprite->setTextureRect({0.0F, 0.0F, (float)chunksWidth, (float)chunksHeight});

    // Peform misc updates
    _eventsMap->removeAllChildren();
    updatePosition();  // Only fired if already active, which can happen in specific cases
    updateBookmark();
    adjustForSkill();  // Updates all metablocks as well
}

void GameMap::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    // TODO: does more stuff in teleport view

    if (_active)
    {
        _vectorNode->clear();
        auto signalAlpha   = (float)fmod(utils::gettime(), 1.0);
        Point signalCenter = _playerIcon->getPosition();
        signalCenter.y += math_util::getScaledHeight(_playerIcon) * 0.5F;
        _vectorNode->drawCircle(signalCenter, signalAlpha * 30.0F, 0.0F, 20, false,
                                {0.0F, 0.0F, 0.0F, 1.0F - signalAlpha});
    }

    Node::draw(renderer, transform, flags);
}

void GameMap::update(float deltaTime)
{
    Node::update(deltaTime);
    auto player = Player::getMain();
    _playerIcon->setPosition(getMapPointAtNodePoint(player->getPosition(), false));
    _playerIcon->setFlippedX(player->getLookDirection() == -1);
}

void GameMap::updateTweenAction(float value, std::string_view key)
{
    int32_t chunkIndex;
    std::from_chars(key.data(), key.data() + key.size(), chunkIndex);
    auto offset = (ssize_t)chunkIndex * 4 + 3;  // Target alpha pixel

    if (offset >= 0 && offset < _chunkMap.size)
    {
        auto width = _zone->getChunkCountX();
        _chunkMap.pixels[offset] = (uint8_t)(value * 255.0F);
        _chunkMap.texture->updateWithSubData(_chunkMap.pixels + offset - 3, chunkIndex % width, chunkIndex / width, 1, 1);
    }
}

void GameMap::updatePosition()
{
    if (!_active)
    {
        // This function gets called on show() so we can afford to be lazy.
        // Just make sure that the initial state does not depend on anything set here.
        return;
    }

    auto& winSize = _director->getWinSize();
    auto viewport = _director->getSafeAreaRect();
    Size mapSize(MAX(MIN_MAP_WIDTH, viewport.size.width - 320.0F - INFO_PANEL_WIDTH), 332.0F);
    Size infoSize(INFO_PANEL_WIDTH, mapSize.height - 10.0F);
    _panel->setSize(mapSize.width + 20.0F, mapSize.height + 20.0F);
    _infoPanel->setSize(infoSize.width, infoSize.height);
    _infoPanel->setPosition(mapSize.width, mapSize.height * 0.5F);
    _titleLabel->setPosition(mapSize.width * 0.5F, mapSize.height - 20.0F);
    math_util::scaleToSize(_surfaceMapSprite, mapSize);
    math_util::scaleToSize(_chunkMapSprite, mapSize);
    _metaMap->setContentSize(mapSize);
    _eventsMap->setContentSize(mapSize);

    // Update content size
    auto rect = _panel->getBoundingBox().unionWithRect(_infoPanel->getBoundingBox());
    setContentSize(rect.size);
    setPosition((winSize.width + _contentSize.width) * 0.5F, viewport.getMinY() + 30.0F);
    setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);

    // Update buttons
    Size buttonSize(infoSize.width - 25.0F, (infoSize.height * 0.5F - 20.0F) * 0.25F);
    _bookmarkButton->scaleToSize(buttonSize, true);
    _bookmarkButton->setPosition(10.0F, 25.0F);
    _snapshotButton->scaleToSize(buttonSize, true);
    _snapshotButton->setPosition(10.0F, _bookmarkButton->getBoundingBox().getMaxY() + 2.0F);
    _rangefinderButton->scaleToSize(buttonSize, true);
    _rangefinderButton->setPosition(10.0F, _snapshotButton->getBoundingBox().getMaxY() + 2.0F);

    // Force reposition info labels
    onZoneStatusChanged();
}

void GameMap::updateBookmark()
{
    auto color = _zone->isBookmarked() ? Color3B::YELLOW : color_util::rgbToColor(kButtonColor);
    _bookmarkButton->getForegroundSprite()->setColor(color);
}

void GameMap::updateAllMetaBlocks()
{
    _metaMap->removeAllChildren();

    for (auto& entry : _zone->getMetaBlocks())
    {
        updateMetaBlock(entry.first, entry.second);
    }
}

void GameMap::updateMetaBlock(int32_t index, MetaBlock* metaBlock)
{
    if (auto child = _metaMap->getChildByTag(index))
    {
        child->removeFromParent();
    }

    // NOTE: Teleporter icons have a different scale in teleport mode (TBA)
    Sprite* sprite = nullptr;
    auto& metadata = metaBlock->getMetadata();
    auto item      = metaBlock->getItem();
    auto z         = 0;

    switch (item->getCode())
    {
    case item_codes::TELEPORTER:
    {
        if (Player::getMain()->canSeeTeleporters())
        {
            sprite = Sprite::createWithSpriteFrameName("map/portal-blue");
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
            sprite->setScale(0.75F * 0.7F * (metaBlock->isOwnedByPlayer() ? 1.3F : 1.0F));
        }

        break;
    }
    case item_codes::ZONE_TELEPORTER:
    {
        sprite = Sprite::createWithSpriteFrameName("map/portal-orange");
        sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
        sprite->setScale(0.75F * 0.7F);
        break;
    }
    case item_codes::PLAQUE:
    case item_codes::RUBY_PLAQUE:
    case item_codes::LANDMARK_PLAQUE:
    case item_codes::DISH_COMPETITION_5:
    case item_codes::DISH_COMPETITION_20:
    case item_codes::DISH_COMPETITION_25:
    {
        auto text = map_util::getString(metadata, "n", map_util::getString(metadata, "pn"));

        if (!text.empty())
        {
            // Create marker sprite
            sprite = Sprite::createWithSpriteFrameName("map/marker");
            sprite->setCascadeOpacityEnabled(true);
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
            sprite->setScale(0.75F);
            z = 2;

            // Create name label
            auto label = MultiLabel::createWithBMFont("console+hd.fnt", text);
            label->setPositionX(sprite->getContentSize().width * 0.5F);
            label->setScale(0.666F);
            label->setColor(color_util::lerpColor(label->getColor(), Color3B::BLACK, 0.75F));
            sprite->addChild(label, 1);

            // Create name background
            auto background = Sprite::createWithSpriteFrameName("white-80-percent");
            background->setPosition(label->getPosition());
            background->setColor(item->getColor());
            math_util::scaleToSize(background, label->getContentSize() * label->getScale() + Size::ONE * 5.0F);
            sprite->addChild(background);
        }

        break;
    }
    default:  // Protector check
    {
        if (metaBlock->isOwnedByPlayer() && item->getField() > 0)
        {
            sprite = Sprite::createWithSpriteFrameName("map/dish");
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
            sprite->setScale(0.75F * 0.7F);
            z = 1;
        }
    }
    }

    if (sprite)
    {
        sprite->setTag(index);
        sprite->setPositionNormalized(getMapPointAtBlockPoint(Point(metaBlock->getX(), metaBlock->getY())));
        _metaMap->addChild(sprite, z);
    }

    _dirtyMetaBlocks.erase(index);
}

void GameMap::adjustForSkill()
{
    auto player = Player::getMain();
    _rangefinderButton->setVisible(player->canSeeProtectorRanges());
    _chunkMapSprite->setVisible(player->canSeeExploredAreas());
    updateAllMetaBlocks();
}

void GameMap::toggle()
{
    if (_active)
    {
        dismiss();
    }
    else
    {
        show();
    }
}

void GameMap::show()
{
    if (!_active)
    {
        stopAllActionsByTag(TOGGLE_ACTION_TAG);
        auto action = Spawn::createWithTwoActions(FadeIn::create(0.0678F), ScaleTo::create(0.0678F, 1.0F));
        action->setTag(TOGGLE_ACTION_TAG);
        runAction(action);
        _active = true;
        updatePosition();

        for (auto index : _dirtyMetaBlocks)
        {
            onMetaBlockChanged(index);
        }

        setVisible(true);
    }
}

void GameMap::dismiss()
{
    if (_active)
    {
        stopAllActionsByTag(TOGGLE_ACTION_TAG);
        auto fadeOut  = Spawn::createWithTwoActions(FadeOut::create(0.0678F), ScaleTo::create(0.0678F, 0.888F));
        auto hideNode = CallFuncN::create([](Node* node) { node->setVisible(false); });
        auto sequence = Sequence::createWithTwoActions(fadeOut, hideNode);
        sequence->setTag(TOGGLE_ACTION_TAG);
        runAction(sequence);
        _active = false;
    }
}

void GameMap::onChunkExplored(int32_t index)
{
    if (!_active)
    {
        // Update pixel immediately if map is hidden
        auto offset = (ssize_t)index * 4 + 3;  // Target alpha pixel

        if (offset >= 0 && offset < _chunkMap.size)
        {
            auto width = _zone->getChunkCountX();
            _chunkMap.pixels[offset] = 0x0;
            _chunkMap.texture->updateWithSubData(_chunkMap.pixels + offset - 3, index % width, index / width, 1, 1);
        }
    }
    else
    {
        // Run tween action to smoothly fade out chunk pixel
        stopActionByTag(index);
        auto action = ActionTween::create(1.0F, std::to_string(index), 0.5F, 0.0F);
        action->setTag(index);
        runAction(action);
    }
}

void GameMap::onMetaBlockChanged(int32_t index)
{
    if (_active)
    {
        if (auto metaBlock = _zone->getMetaBlock(index))
        {
            updateMetaBlock(index, metaBlock);
        }
    }
    else
    {
        _dirtyMetaBlocks.insert(index);
    }
}

void GameMap::onPlayerDeath()
{
    // NOTE: Death positions were originally stored in WorldZone, but since they don't seem
    // to be used for anything other than the game map, we just listen to the death event
    // and create a new death icon directly.
    // (I've also tweaked the death sprite's scale & position a little bit)
    auto position = Player::getMain()->getBlockPositionPoint();
    auto sprite   = Sprite::createWithSpriteFrameName("map/death");
    sprite->setPositionNormalized(getMapPointAtBlockPoint(position));
    sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    sprite->setScale(0.6F);
    _eventsMap->addChild(sprite);
}

void GameMap::onZoneStatusChanged()
{
    if (!_active)
    {
        // NOTE: We can only get away with not using a dirty flag here because
        // show() calls updatePosition() which will call this function again.
        return;
    }

    auto acidityType = map_util::getString(_zone->getBiomeConfig(), "acidity", "Acidity");
    auto explored    = (int)floorf((float)_zone->getChunksExploredCount() / _zone->getChunkCount() * 100.0F);
    auto acidity     = (int)floorf(_zone->getAcidity() * 100.0F);
    std::vector<std::pair<std::string, std::string>> data = {
        {"Explored:", std::format("{}%", explored)}, {std::format("{}:", acidityType), std::format("{}%", acidity)}};

    // Append discovered machine part info
    auto& parts = _zone->getMachinePartsDiscovered();

    for (auto i = 0; i < parts.size(); i++)
    {
        auto& entry  = *std::next(parts.begin(), parts.size() - 1 - i);  // Reverse order
        auto& config = GameConfig::getMain()->getData();
        auto& key    = entry.first;
        auto count   = entry.second.asValueVector().size();
        auto name    = map_util::getString(config, std::format("machines.{}.name", key), key);
        auto parts   = map_util::getInt32(config, std::format("machines.{}.parts", key));
        data.push_back({std::format("{}:", name), std::format("{} of {}", count, parts)});
    }

    // Update info labels
    auto currentY = _infoPanel->getContentSize().height - 12.0F;

    for (auto i = 0; i < data.size(); i++)
    {
        // Create or update key label
        auto labelIndex = i * 2;
        Label* keyLabel = nullptr;
        auto& key       = data[i].first;

        if (labelIndex < _infoLabels.size())
        {
            keyLabel = _infoLabels[labelIndex];
            keyLabel->setString(key);
        }
        else
        {
            keyLabel = MultiLabel::createWithBMFont("console+hd.fnt", key);
            keyLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
            keyLabel->setScale(0.42F);
            keyLabel->setColor(color_util::rgbToColor(0xD2D2D2));
            _infoPanel->addChild(keyLabel, 2);
            _infoLabels.push_back(keyLabel);
        }

        keyLabel->setPosition(9.0F, currentY);
        currentY -= math_util::getScaledHeight(keyLabel) + 2.0F;

        // Create or update value label
        labelIndex++;
        Label* valueLabel = nullptr;
        auto& value       = data[i].second;

        if (labelIndex < _infoLabels.size())
        {
            valueLabel = _infoLabels[labelIndex];
            valueLabel->setString(value);
        }
        else
        {
            valueLabel = MultiLabel::createWithBMFont("console+hd.fnt", value);
            valueLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
            valueLabel->setScale(0.5F);
            _infoPanel->addChild(valueLabel, 2);
            _infoLabels.push_back(valueLabel);
        }

        valueLabel->setPosition(9.0F, currentY);
        currentY -= math_util::getScaledHeight(valueLabel) + 5.0F;
    }

    // Remove excess labels
    auto start  = (ssize_t)data.size() * 2;
    auto excess = (ssize_t)_infoLabels.size() - start;

    while (excess > 0)
    {
        _infoLabels[start]->removeFromParent();
        _infoLabels.erase(_infoLabels.begin() + start);
        excess--;
    }
}

Point GameMap::getMapPointAtNodePoint(const Point& point, bool normalized) const
{
    // Same as WorldZone::getBlockPointAtNodePoint but without floor() so that it is smooth
    return getMapPointAtBlockPoint({point.x / BLOCK_SIZE, -point.y / BLOCK_SIZE}, normalized);
}

Point GameMap::getMapPointAtBlockPoint(Point point, bool normalized) const
{
    point.y = _zone->getBlocksHeight() - point.y;

    if (normalized)
    {
        Size size(_zone->getBlocksWidth(), _zone->getBlocksHeight());
        return point / size;
    }
    else
    {
        return convertToNodeSpace(_surfaceMapSprite->convertToWorldSpace(point));
    }
}

}  // namespace opendw

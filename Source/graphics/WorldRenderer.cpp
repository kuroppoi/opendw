#include "WorldRenderer.h"

#include "entity/Entity.h"
#include "entity/EntityAnimated.h"
#include "entity/EntityConfig.h"
#include "graphics/backend/MaskedSprite.h"
#include "graphics/backend/MaskedSpriteBatchNode.h"
#include "graphics/CavernRenderer.h"
#include "graphics/Lightmapper.h"
#include "graphics/SkyRenderer.h"
#include "graphics/WorldLayerRenderer.h"
#include "util/MathUtil.h"
#include "zone/BaseBlock.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"
#include "GameConfig.h"
#include "GameManager.h"
#include "Item.h"
#include "Player.h"

#define MAX_BLOCK_RENDER_FRAME 0.1
#define FX_PROCESS_INTERVAL    0.2
#define LIQUID_CYCLE_INTERVAL  0.333

USING_NS_AX;

namespace opendw
{

WorldRenderer* WorldRenderer::createWithZone(WorldZone* zone)
{
    CREATE_INIT(WorldRenderer, initWithZone, zone);
}

bool WorldRenderer::initWithZone(WorldZone* zone)
{
    if (!Node::init())
    {
        return false;
    }

    _zone            = zone;
    _worldScale      = 1.0F;
    _nextFX          = 0.0F;
    _fxFrame         = 0;
    _nextLiquidCycle = 0.0F;
    _liquidFrame     = 0;

    // Create background
    _background = Node::create();
    _cavern     = CavernRenderer::createWithZone(zone);
    _sky        = SkyRenderer::createWithZone(zone);
    _background->addChild(_cavern);
    _background->addChild(_sky);
    addChild(_background);

    // Create foreground
    _foreground = Node::create();
    addChild(_foreground);

    // Create lightmapper
    _lightmapper = Lightmapper::createWithZone(zone);
    addChild(_lightmapper);

    // 0x10007D1D2: Create base layer renderers
    _baseBiomeBlocksNode = createLayerRenderer("baseBiome", BlockLayer::BASE, "front-0+hd2.png", true);
    _baseBlocksNode      = createLayerRenderer("base", BlockLayer::BASE, "base+hd2.png");
    _baseAccentsNode     = createLayerRenderer("baseAccents", BlockLayer::BASE, "accents+hd2.png");
    _baseBlocksNode->addAltRenderer(_baseBiomeBlocksNode);
    _baseBlocksNode->addAltRenderer(_baseAccentsNode);

    // 0x10007D295: Create back layer renderers
    _backBiomeBlocksNode = createLayerRenderer("backBiome", BlockLayer::BACK, "back+hd2.png", true);
    _backBlocksNode      = createLayerRenderer("back", BlockLayer::BACK, "back+hd2.png");
    _backAccentsNode     = createLayerRenderer("backAccents", BlockLayer::BACK, "accents+hd2.png");
    _backBlocksNode->addAltRenderer(_backBiomeBlocksNode);
    _backBlocksNode->addAltRenderer(_backAccentsNode);

    // 0x10007D359: Create front layer renderers
    auto frontBackgroundNode = createLayerRenderer("front-0b", BlockLayer::FRONT, "front-0+hd2.png");
    auto frontAltNode        = createLayerRenderer("front-1", BlockLayer::FRONT, "front-1+hd2.png");
    _frontBlocksNode         = createLayerRenderer("front-0a", BlockLayer::FRONT, "front-0+hd2.png");
    _frontBiomeBlocksNode    = createLayerRenderer("frontBiome", BlockLayer::FRONT, "front-0+hd2.png", true);
    auto frontAltWholeNode   = createLayerRenderer("front-alt-whole", BlockLayer::FRONT, "front-whole+hd2.png");
    _signsNode               = createLayerRenderer("signs", BlockLayer::FRONT, "signs+hd2.png");
    _frontQualityBlocksNode  = createLayerRenderer("frontQuality", BlockLayer::FRONT, "front-quality+hd2.png");
    _frontBlocksNode->setPlaceBackgroundsInAlt(true);
    _frontBlocksNode->addAltRenderer(frontBackgroundNode);
    _frontBlocksNode->addAltRenderer(frontAltNode);
    _frontBlocksNode->addAltRenderer(_frontBiomeBlocksNode);
    _frontBlocksNode->addAltRenderer(frontAltWholeNode);
    _frontBlocksNode->addAltRenderer(_signsNode);
    _frontBlocksNode->addAltRenderer(_frontQualityBlocksNode);

    // 0x10007D566: Create entity nodes
    _entitiesNode = SpriteBatchNode::create("entities+hd2.png");
    _foreground->addChild(_entitiesNode, getNextZIndex());
    auto& winSize = _director->getWinSize();
    _animatedEntitiesNode = Node::create();
    _foreground->addChild(_animatedEntitiesNode, getNextZIndex());

    // 0x10007D7F2: Create liquid layer renderer
    _liquidBlocksNode = createLayerRenderer("liquid", BlockLayer::LIQUID, "liquid+hd2.png");

    // 0x10007D824: Create fronter layer renderers
    _fronterBiomeBlocksNode = createLayerRenderer("fronterBiome", BlockLayer::FRONT, "front-whole+hd2.png", true);
    _fronterBlocksNode      = createLayerRenderer("fronter", BlockLayer::FRONT, "front-whole+hd2.png");
    auto fronterBaseNode    = createLayerRenderer("fronterBase", BlockLayer::FRONT, "base+hd2.png");
    _fronterAccentsNode     = createLayerRenderer("fronterAccents", BlockLayer::FRONT, "accents+hd2.png");
    _fronterBlocksNode->addAltRenderer(_fronterBiomeBlocksNode);
    _fronterBlocksNode->addAltRenderer(fronterBaseNode);
    _fronterBlocksNode->addAltRenderer(_fronterAccentsNode);

    // 0x10007D8FC: Create fronter entity nodes
    _animatedGhostlyEntitiesNode = Node::create();
    _foreground->addChild(_animatedGhostlyEntitiesNode, getNextZIndex());

    // Create misc nodes
    _textNode = Node::create();  // Originally SpriteBatchNode but we cannot add labels to those
    _foreground->addChild(_textNode, getNextZIndex());

    // 0x10007DE0F: Precompute corner masks
    _wholenessCornerMasks.reserve(256);
    _continuityCornerMasks.reserve(256);

    for (auto i = 0; i < 256; i++)
    {
        uint8_t topAndRight    = BaseBlock::CONTINUITY_TOP | BaseBlock::CONTINUITY_RIGHT;
        uint8_t bottomAndRight = BaseBlock::CONTINUITY_BOTTOM | BaseBlock::CONTINUITY_RIGHT;
        uint8_t bottomAndLeft  = BaseBlock::CONTINUITY_BOTTOM | BaseBlock::CONTINUITY_LEFT;
        uint8_t topAndLeft     = BaseBlock::CONTINUITY_TOP | BaseBlock::CONTINUITY_LEFT;

        // 0x10007DE0F: Wholeness corner masks
        std::vector<uint16_t> wholeness;

        // If continuous with top right neighbor but not with top & right neighbors
        if ((i & (topAndRight | BaseBlock::CONTINUITY_TOP_RIGHT)) == BaseBlock::CONTINUITY_TOP_RIGHT)
        {
            wholeness.push_back(0);
        }

        // If continuous with bottom right neighbor but not with bottom & right neighbors
        if ((i & (bottomAndRight | BaseBlock::CONTINUITY_BOTTOM_RIGHT)) == BaseBlock::CONTINUITY_BOTTOM_RIGHT)
        {
            wholeness.push_back(90);
        }

        // If continuous with bottom left neighbor but not with bottom & left neighbors
        if ((i & (bottomAndLeft | BaseBlock::CONTINUITY_BOTTOM_LEFT)) == BaseBlock::CONTINUITY_BOTTOM_LEFT)
        {
            wholeness.push_back(180);
        }

        // If continuous with top left neighbor but not with top & left neighbors
        if ((i & (topAndLeft | BaseBlock::CONTINUITY_TOP_LEFT)) == BaseBlock::CONTINUITY_TOP_LEFT)
        {
            wholeness.push_back(270);
        }

        _wholenessCornerMasks.push_back(wholeness);

        // 0x10007DF0D: Continuity corner masks
        std::vector<uint16_t> continuity;

        // If continuous with top & right neighbors but not with top right neighbor
        if ((i & (topAndRight | BaseBlock::CONTINUITY_TOP_RIGHT)) == topAndRight)
        {
            continuity.push_back(0);  // 0 degrees = top right corner
        }

        // If continuous with bottom & right neighbors but not with buttom right neighbor
        if ((i & (bottomAndRight | BaseBlock::CONTINUITY_BOTTOM_RIGHT)) == bottomAndRight)
        {
            continuity.push_back(90);  // 90 degrees = bottom right corner
        }

        // If continuous with bottom & left neighbors but not with bottom left neighbor
        if ((i & (bottomAndLeft | BaseBlock::CONTINUITY_BOTTOM_LEFT)) == bottomAndLeft)
        {
            continuity.push_back(180);  // 180 degrees = bottom left
        }

        // If continuous with top & left neighbors but not with top left neighbor
        if ((i & (topAndLeft | BaseBlock::CONTINUITY_TOP_LEFT)) == topAndLeft)
        {
            continuity.push_back(270);  // 270 degrees = top left
        }

        _continuityCornerMasks.push_back(continuity);
    }

    AXLOGI("[WorldRenderer] Initialized");
    return true;
}

void WorldRenderer::ready()
{
    _sky->rebuild();
    _cavern->rebuild();
    updateViewport(0.0F);
}

void WorldRenderer::clear()
{
    _sky->clear();
    _cavern->clear();
    _renderQueue.clear();

    for (auto& child : _foreground->getChildren())
    {
        auto renderer = dynamic_cast<WorldLayerRenderer*>(child);

        if (renderer)
        {
            renderer->clear();
        }
        else
        {
            child->removeAllChildren();
        }
    }
}

void WorldRenderer::update(float deltaTime)
{
    setVisible(true);
    updateViewport(deltaTime);
    _sky->update(deltaTime);
    _cavern->update(deltaTime);
    updateBlocks();

    // 0x10007EABB: Process block effects (sprite animations, emitters, etc.)
    if (utils::gettime() >= _nextFX)
    {
        processEffects();
        _fxFrame++;
        _nextFX = utils::gettime() + FX_PROCESS_INTERVAL;
    }

    // 0x10007EB28: Update liquid animation
    if (utils::gettime() >= _nextLiquidCycle)
    {
        for (auto& child : _liquidBlocksNode->getBatchNode()->getChildren())
        {
            auto sprite = static_cast<MaskedSprite*>(child);
            auto tag    = sprite->getTag();
            auto liquid = tag & 0xFF;

            if (liquid == 0)
            {
                continue;
            }

            auto item = GameManager::getInstance()->getConfig()->getItemForCode(liquid);
            AX_ASSERT(item);
            auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(std::format(
                "{}-{}", item->getName(), (_liquidFrame % 3) + 1));  // TODO: precomputing may speed things up

            if (!frame)
            {
                continue;
            }

            auto& frameRect   = frame->getRect();
            auto& textureRect = sprite->getTextureRect();
            auto tileX        = (tag >> 16) & 0xFF;
            auto tileY        = (tag >> 8) & 0xFF;
            sprite->setTextureRect({frameRect.origin.x + tileX * BLOCK_SIZE,
                                    frameRect.origin.y + tileY * BLOCK_SIZE + (BLOCK_SIZE - textureRect.size.height),
                                    textureRect.size.width, textureRect.size.height});
        }

        _liquidFrame++;
        _nextLiquidCycle = utils::gettime() + LIQUID_CYCLE_INTERVAL;
    }

    _lightmapper->update(deltaTime);
}

void WorldRenderer::updateBlocks()
{
    // TODO
    arrangeBlockSprites();
    renderBlockSprites();
}

void WorldRenderer::loadBiome(const std::string& biome)
{
    auto key     = std::format("biome-{}+hd2.png", biome == "plain" ? "temperate" : biome);
    auto texture = _director->getTextureCache()->addImage(key);

    // Set texture for all biome renderers
    for (auto& child : _foreground->getChildren())
    {
        auto renderer = dynamic_cast<WorldLayerRenderer*>(child);

        if (renderer && renderer->isBiomeRenderer())
        {
            renderer->clear();
            renderer->getBatchNode()->setTexture(texture);
        }
    }
}

void WorldRenderer::arrangeBlockSprites()
{
    auto upperLeft   = _zone->getUpperLeftScreenBlockPoint();
    auto lowerRight  = _zone->getLowerRightScreenBlockPoint();
    auto arrangeUL   = Point(upperLeft.x - 4.0F, upperLeft.y - 1.0F);
    auto arrangeLR   = Point(lowerRight.x + 1.0F, lowerRight.y + 4.0F);
    auto arrangeRect = Rect(arrangeUL, arrangeLR - arrangeUL);

    // 0x10008058F: Do nothing if arrange rect has not changed since last update
    if (arrangeRect.equals(_lastArrangeRect))
    {
        return;
    }

    // 0x1000805F6: Find intersection between arrange rect and previous arrange rect
    auto intersection = math_util::getRectIntersection(arrangeRect, _lastArrangeRect);

    // 0x100080668: Add new blocks to the render queue
    for (auto x = arrangeRect.getMinX(); x <= arrangeRect.getMaxX(); x++)
    {
        for (auto y = arrangeRect.getMinY(); y <= arrangeRect.getMaxY(); y++)
        {
            // Only add blocks that weren't already rendered last time
            if (!intersection.containsPoint({x, y}))
            {
                if (auto block = _zone->getBlockAt((uint16_t)x, (uint16_t)y))
                {
                    _renderQueue.pushBack(block);
                }
            }
        }
    }

    // 0x100080792: Clear blocks that are no longer visible
    for (auto x = _lastArrangeRect.getMinX(); x <= _lastArrangeRect.getMaxX(); x++)
    {
        for (auto y = _lastArrangeRect.getMinY(); y <= _lastArrangeRect.getMaxY(); y++)
        {
            if (!intersection.containsPoint({x, y}))
            {
                if (auto block = _zone->getBlockAt((uint16_t)x, (uint16_t)y))
                {
                    block->clearFromWorld();
                }
            }
        }
    }

    // TODO: is there a reason to keep these separate?
    _blockRect       = arrangeRect;
    _lastArrangeRect = arrangeRect;
}

void WorldRenderer::renderBlockSprites()
{
    if (_renderQueue.empty())
    {
        return;
    }

    double start = utils::gettime();

    while (!_renderQueue.empty())
    {
        // 0x100080AE5: Quit placing blocks if this frame is taking too long
        if (utils::gettime() >= start + MAX_BLOCK_RENDER_FRAME)
        {
            break;
        }

        auto block = _renderQueue[0];
        block->setRendering(true);
        block->recycleSprites();
        block->postPlace();

        if (block->getFrontItem()->isWhole())
        {
            _fronterBlocksNode->placeBlock(block);
        }
        else
        {
            _frontBlocksNode->placeBlock(block);
        }

        // NOTE: Occlusion culling is handled by WorldLayerRenderer.
        // Also, there shouldn't be a need to render liquid blocks here.
        _backBlocksNode->placeBlock(block);
        _baseBlocksNode->placeBlock(block);
        block->setRendering(false);
        _renderQueue.erase(0);
    }
}

void WorldRenderer::processEffects()
{
    // TODO: use captured screen blocks which is set by LightMapper for some mysterious reason
    for (auto x = _blockRect.getMinX(); x < _blockRect.getMaxX(); x++)
    {
        for (auto y = _blockRect.getMinY(); y < _blockRect.getMaxY(); y++)
        {
            auto block = _zone->getBlockAt((uint16_t)x, (uint16_t)y);

            if (!block)
            {
                continue;
            }

            // Base, back, front
            // NOTE: the original implementation is a lot less flexible and doesn't support all layers
            for (uint8_t i = 0; i < 3; i++)
            {
                auto layer                   = static_cast<BlockLayer>(i + 1);
                WorldLayerRenderer* renderer = nullptr;
                Item* item                   = nullptr;
                uint8_t mod                  = 0;

                switch (layer)
                {
                case BlockLayer::BASE:
                    renderer = _baseBlocksNode;
                    item     = block->getBaseItem();
                    break;
                case BlockLayer::BACK:
                    renderer = _backBlocksNode;
                    item     = block->getBackItem();
                    mod      = block->getBackMod();
                    break;
                case BlockLayer::FRONT:
                    renderer = _frontBlocksNode;
                    item     = block->getFrontItem();
                    mod      = block->getFrontMod();
                    break;
                }

                // Cycle sprite animation
                auto& spriteAnimation = item->getSpriteAnimation();

                if (!spriteAnimation.empty())
                {
                    auto tag = WorldLayerRenderer::ANIMATED_SPRITE_TAG - i;
                    block->recycleSpriteWithTag(tag);
                    auto frame  = spriteAnimation[_fxFrame % spriteAnimation.size()];
                    auto sprite = renderer->placeSprite(block, nullptr, frame, false, true, item->getModType(), mod, 2);
                    sprite->setColor(item->getSpriteAnimationColor());
                    sprite->setTag(tag);
                }

                // Cycle continuity animation
                auto& continuityAnimation = item->getSpriteContinuityAnimation();

                if (!continuityAnimation.empty())
                {
                    auto tag         = WorldLayerRenderer::ANIMATED_SPRITE_TAG - 3 - i;
                    auto continuity  = block->getContinuityForLayer(layer);
                    auto& spriteInfo = continuityAnimation[continuity & 0xF].back();
                    auto& options    = spriteInfo.options;
                    auto frame       = options[_fxFrame % options.size()];
                    auto rotation    = spriteInfo.rotation;
                    block->recycleSpriteWithTag(tag);
                    auto sprite =
                        renderer->placeSprite(block, nullptr, frame, true, true, ModType::ROTATION_DEGREES, rotation,
                                              item->getSpriteZ() + 1);  // HACK: always render on top
                    sprite->setOpacity(item->getSpriteContinuityAnimationOpacity());
                    sprite->setTag(tag);
                }

                // TODO: emitters
            }
        }
    }
}

void WorldRenderer::updateLiquidInBlock(BaseBlock* block)
{
    auto item = block->getLiquidItem();

    if (item)
    {
        auto mod = block->getLiquidMod();

        if (mod > 0)
        {
            auto sprite = block->getTopSpriteForLayer(BlockLayer::LIQUID);
            auto frame  = item->getSpriteFrame();
            auto z      = sprite ? sprite->getLocalZOrder() : 0;
            _liquidBlocksNode->placeSprite(block, sprite, frame, true, true, ModType::NONE, mod, z, !sprite);
            return;
        }
    }

    block->recycleSprites(BlockLayer::LIQUID);
}

void WorldRenderer::updateViewport(float deltaTime)
{
    auto viewport = getViewportPosition();

    // TODO: clean up
    auto player        = GameManager::getInstance()->getPlayer();
    auto biome         = _zone->getBiomeType();
    auto cameraPos     = player->getPosition() * BLOCK_SIZE;
    bool cavernVisible = biome == Biome::DEEP ||
                         (biome != Biome::SPACE && player->getBlockPosition().y > _zone->getSurfaceBottom() + 20);
    _sky->setVisible(!cavernVisible);
    _cavern->setVisible(cavernVisible);
    _foreground->setPosition(-viewport);
    _foreground->setScale(_worldScale);
    _sky->setViewPosition(cameraPos);
    _sky->setViewScale(_worldScale);
    _cavern->setViewPosition(cameraPos);
    _cavern->setViewScale(_worldScale);
}

Point WorldRenderer::getViewportPosition() const
{
    // TODO: finish
    auto& winSize   = _director->getWinSize();
    auto& cameraPos = GameManager::getInstance()->getPlayer()->getPosition();
    auto position   = cameraPos * BLOCK_SIZE * _worldScale;
    return position - winSize * 0.5F;
}

WorldLayerRenderer* WorldRenderer::createLayerRenderer(const std::string& name,
                                                       BlockLayer layer,
                                                       const std::string& texture,
                                                       bool biomeRenderer,
                                                       int z)
{
    auto cache       = _director->getTextureCache();
    auto image       = cache->addImage(texture);
    auto maskTexture = cache->addImage("masks+hd2.png");
    auto batchNode   = MaskedSpriteBatchNode::createWithTexture(image, maskTexture, 600);
    auto renderer    = WorldLayerRenderer::createWithLayer(layer, batchNode);
    renderer->setName(name);
    renderer->setBiomeRenderer(biomeRenderer);
    _foreground->addChild(renderer, z == -1 ? getNextZIndex() : z);
    return renderer;
}

int WorldRenderer::getNextZIndex() const
{
    return getChildrenCount() + _foreground->getChildrenCount();
}

bool WorldRenderer::isBlockInViewport(BaseBlock* block) const
{
    if (_zone->getState() != WorldZone::State::ACTIVE)
    {
        return false;
    }

    return _blockRect.containsPoint({(float)block->getX(), (float)block->getY()});
}

void WorldRenderer::queueBlockForRendering(BaseBlock* block)
{
    block->setQueuedAt(utils::gettime());
    _renderQueue.pushBack(block);
}

bool WorldRenderer::hasRenderedAllPlacedBlocks()
{
    if (!_renderQueue.empty())
    {
        for (auto block : _renderQueue)
        {
            if (block->getQueuedAt() < _zone->getDoneWaitingForBlocksAt())
            {
                return false;
            }
        }
    }

    return true;
}

Entity* WorldRenderer::addEntity(int32_t code, const std::string& name, const ValueMap& details)
{
    // Safe: config null check is handled by Entity::createWithConfig
    auto config = GameConfig::getMain()->getEntityForCode(code);
    auto entity = Entity::createWithConfig(config, name, details);

    if (!entity)
    {
        return nullptr;
    }

    auto nameLabel = entity->getNameLabel();

    if (nameLabel)
    {
        _textNode->addChild(nameLabel);
    }

    if (dynamic_cast<EntityAnimated*>(entity))
    {
        if (config->isGhostly())
        {
            _animatedGhostlyEntitiesNode->addChild(entity);
        }
        else
        {
            _animatedEntitiesNode->addChild(entity);
        }
    }
    else if (entity->getTexture() == _entitiesNode->getTexture())
    {
        _entitiesNode->addChild(entity);
    }

    return entity;
}

void WorldRenderer::setWorldScale(float scale)
{
    if (_worldScale != scale)
    {
        _worldScale = scale;
        updateViewport(1.0F);
    }
}

ax::Point WorldRenderer::getNodePointForScreenPoint(const ax::Point& point) const
{
    return _foreground->convertToNodeSpace(point);
}

}  // namespace opendw

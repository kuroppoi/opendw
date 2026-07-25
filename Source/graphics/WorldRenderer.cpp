#include "WorldRenderer.h"

#include "base/Emitter.h"
#include "base/GameConfig.h"
#include "base/Item.h"
#include "base/Player.h"
#include "entity/Entity.h"
#include "entity/EntityAnimated.h"
#include "entity/EntityConfig.h"
#include "graphics/backend/MaskedSprite.h"
#include "graphics/backend/MaskedSpriteBatchNode.h"
#include "graphics/CavernRenderer.h"
#include "graphics/Debris.h"
#include "graphics/Lightmapper.h"
#include "graphics/SkyRenderer.h"
#include "graphics/VectorLayer.h"
#include "graphics/WorldLayerRenderer.h"
#include "gui/widget/MultiLabel.h"
#include "physics/ChipmunkBody.h"
#include "physics/ChipmunkSpace.h"
#include "physics/Physical.h"
#include "physics/PhysicsDebugNode.h"
#include "util/AxUtil.h"
#include "util/ColorUtil.h"
#include "util/MathUtil.h"
#include "zone/BaseBlock.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
#include "CommonDefs.h"
#include "GameManager.h"

#define MAX_BLOCK_RENDER_FRAME 0.1
#define FX_PROCESS_INTERVAL    0.2
#define LIQUID_CYCLE_INTERVAL  0.333
#define BLOCK_DEBRIS_INTERVAL  0.0789
#define GLOW_SPRITE_ITERATIONS 3
#define GECK_TUB               880
#define COMPOSTER_CHAMBER      894
#define DEBRIS_POOL_SIZE       2000

USING_NS_AX;

namespace opendw
{

WorldRenderer::~WorldRenderer()
{
    AX_SAFE_RELEASE(_miningCracksAnimation);

    // Free debris pool
    if (_debrisPool)
    {
        for (ssize_t i = 0; i < DEBRIS_POOL_SIZE; i++)
        {
            AX_SAFE_RELEASE(_debrisPool[i]);
        }

        delete[] _debrisPool;
    }
}

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
    auto& winSize         = _director->getWinSize();
    _animatedEntitiesNode = Node::create();
    _foreground->addChild(_animatedEntitiesNode, getNextZIndex());
    _animatedCharactersNode = Node::create();
    _foreground->addChild(_animatedCharactersNode, getNextZIndex());

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
    _effectsNode = SpriteBatchNode::create("effects+hd2.png");
    _foreground->addChild(_effectsNode, getNextZIndex());
    _textNode = Node::create();  // Originally SpriteBatchNode but we cannot add labels to those
    _foreground->addChild(_textNode, getNextZIndex());
    _guiNode = Node::create();
    _foreground->addChild(_guiNode, getNextZIndex());
    _glowNode = Node::create();
    _foreground->addChild(_glowNode, getNextZIndex());
    _vectorLayer = VectorLayer::create();
    _foreground->addChild(_vectorLayer, getNextZIndex());
    _physicsDebugNode = PhysicsDebugNode::create();
    _physicsDebugNode->setVisible(false);
    _foreground->addChild(_physicsDebugNode, getNextZIndex());

    // 0x10007DCC9: Create mining cracks animation
    auto cache = SpriteFrameCache::getInstance();
    Vector<SpriteFrame*> frames;
    frames.pushBack(cache->getSpriteFrameByName("crack-1"));
    frames.pushBack(cache->getSpriteFrameByName("crack-2"));
    frames.pushBack(cache->getSpriteFrameByName("crack-3"));
    _miningCracksAnimation = Animation::createWithSpriteFrames(frames, 1.0F);
    AX_SAFE_RETAIN(_miningCracksAnimation);

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

    sMain = this;
    AXLOGI("[WorldRenderer] Initialized");
    return true;
}

void WorldRenderer::ready()
{
    _sky->rebuild();
    _cavern->rebuild();
    _initialArrange = true;
    updateViewport(0.0F);

    // 0x10007E86A: Preallocate debris pool
    if (!_debrisPool)
    {
        _debrisPool = new Debris*[DEBRIS_POOL_SIZE];

        for (ssize_t i = 0; i < DEBRIS_POOL_SIZE; i++)
        {
            auto debris = Debris::createWithZone(_zone);
            debris->retain();
            debris->setPoolIndex(i);
            _debrisPool[i] = debris;
        }
    }
}

void WorldRenderer::clear()
{
    _earthquake = 0.0F;
    _explosion  = 0.0F;
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

    // Clear remaining debris
    while (_freeDebrisIndex != 0)
    {
        _debrisPool[_freeDebrisIndex - 1]->removeFromParent();
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
            if (!child->isVisible())
            {
                continue;
            }

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

    // 0x10007EFE8: Update game objects
    // TODO: there's a lot more going on here...
    for (auto body : _zone->getSpace()->getBodies())
    {
        if (body->getType() == CP_BODY_TYPE_STATIC)
        {
            continue;
        }

        if (auto entity = dynamic_cast<Entity*>(body->getUserData()))
        {
            auto& contentSize = entity->getContentSize();
            auto size         = MAX(contentSize.x, contentSize.y) * 2.0F;  // HACK: Overcompensate for rotation
            auto rect         = math_util::growRect(_visibleRect, {size, size});
            auto onscreen     = rect.containsPoint(body->getPosition());
            entity->updateOnscreen(deltaTime, onscreen);
        }
        else
        {
            auto node = static_cast<Node*>(body->getUserData());
            node->setPosition(body->getPosition());
            node->setRotation(MATH_RAD_TO_DEG(body->getAngle()));
        }
    }

    // 0x10007F736: Update earthquake
    auto earthquakeForce     = clampf(_earthquake * 0.125F, 0.0F, 1.0F);
    auto earthquakeProximity = 0.0F;  // Current proximity to earthquake epicenter, clamped between [0.1, 1.0]
    auto screenShake         = 0.0F;

    if (_earthquake > 0.0F)
    {
        _earthquake -= deltaTime;
        auto player   = Player::getMain();
        auto position = player->getPosition();
        auto distance =
            math_util::getDistance(position.x, position.y, _earthquakeEpicenter.x, _earthquakeEpicenter.y) / BLOCK_SIZE;
        earthquakeProximity = clampf(distance / -300.0F + 1.0F, 0.1F, 1.0F);
        auto effectiveForce = earthquakeForce * earthquakeProximity;
        screenShake         = math_util::lerp(0.0F, BLOCK_SIZE * 0.12F, effectiveForce);

        // 0x10007F8A3: Push player around a bit if effective force is great
        // FIXME: For obvious reasons, doing this in the world renderer of all places isn't ideal.
        if (effectiveForce > 0.5F && player->isGrounded() && rand_0_1() < deltaTime)
        {
            auto impulse  = math_util::lerp(15.0F, 50.0F, effectiveForce) * BLOCK_SIZE;
            auto impulseX = random(-impulse, impulse) * 0.5F;
            auto impulseY = random(0.5F, 2.34F) * BLOCK_SIZE;
            player->getPhysical()->getBody()->applyImpulseAtLocalPoint({impulseX, impulseY});
        }
    }

    auto gain = clampf(earthquakeForce * earthquakeProximity * 5.0F, 0.0F, 1.0f);
    AudioManager::getInstance()->setAutoLoopLayer("earthquake", 1.0F, gain);

    // 0x10007FA4B: Update explosion
    if (_explosion > 0.0F)
    {
        _explosion -= deltaTime;
        screenShake += math_util::lerp(0.0F, BLOCK_SIZE, _explosion * 0.5F);
    }

    // 0x10007FAC0: Apply screen shake
    if (screenShake > 0.0F)
    {
        auto shakeX = random(-screenShake, screenShake) * 0.5F;
        auto shakeY = random(-screenShake, screenShake) * 0.5F;
        _foreground->setPosition(_foreground->getPositionX() + shakeX, _foreground->getPositionY() + shakeY);
    }

    _lightmapper->update(deltaTime);
    _sky->setVisible(_lightmapper->isSkyVisible());
    _cavern->setVisible(_lightmapper->isCavernVisible());
}

void WorldRenderer::updateBlocks()
{
    auto& winSize = _director->getWinSize();
    auto origin   = getNodePointForScreenPoint(Point::ZERO);
    auto size     = winSize / _worldScale;
    _visibleRect  = Rect(origin, size);
    arrangeBlockSprites();
    renderBlockSprites();
}

void WorldRenderer::glowSprite(MaskedSprite* sprite)
{
    if (!sprite)
    {
        return;
    }

    // NOTE: Doesn't work well with masks
    for (auto i = 0; i < GLOW_SPRITE_ITERATIONS; i++)
    {
        auto glowSprite = Sprite::createWithTexture(sprite->getTexture());
        glowSprite->setBlendFunc({backend::BlendFactor::ONE_MINUS_SRC_ALPHA, backend::BlendFactor::ONE});
        glowSprite->setTextureRect(sprite->getTextureRect());
        glowSprite->setPosition(sprite->getPosition());
        glowSprite->setRotation(sprite->getRotation());
        glowSprite->setFlippedX(sprite->isFlippedX());
        glowSprite->setScale(sprite->getScale());
        _glowNode->addChild(glowSprite, 20);
        ax_util::fadeOutAndRemove(glowSprite, 0.25F);
    }
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
    if (!_initialArrange && arrangeRect.equals(_lastArrangeRect))
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
            if (_initialArrange || !intersection.containsPoint({x, y}))
            {
                if (auto block = _zone->getBlockAt((int16_t)x, (int16_t)y))
                {
                    _renderQueue.pushBack(block);
                }
            }
        }
    }

    // 0x100080792: Clear blocks that are no longer visible
    if (!_initialArrange)
    {
        for (auto x = _lastArrangeRect.getMinX(); x <= _lastArrangeRect.getMaxX(); x++)
        {
            for (auto y = _lastArrangeRect.getMinY(); y <= _lastArrangeRect.getMaxY(); y++)
            {
                if (!intersection.containsPoint({x, y}))
                {
                    if (auto block = _zone->getBlockAt((int16_t)x, (int16_t)y))
                    {
                        block->clearFromWorld();
                    }
                }
            }
        }
    }

    // TODO: is there a reason to keep these separate?
    _blockRect       = arrangeRect;
    _lastArrangeRect = arrangeRect;
    _initialArrange  = false;
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

        // 0x100080BDF: Determine front node
        auto frontNode = _frontBlocksNode;
        auto frontItem = block->getFrontItem();

        if (block->getFront() > 0)
        {
            if (frontItem->isWhole())
            {
                frontNode = _fronterBlocksNode;
            }
            else if (auto frame = frontItem->getSpriteFrame())
            {
                if (frame->getTexture() == _fronterBlocksNode->getBatchNode()->getTexture() &&
                    frontItem->getSpriteZ() != -1)  // Exception for assembled fossils
                {
                    frontNode = _fronterBlocksNode;
                }
            }
        }

        // NOTE: Occlusion culling is handled by WorldLayerRenderer.
        // Also, there shouldn't be a need to render liquid blocks here.
        frontNode->placeBlock(block);
        _backBlocksNode->placeBlock(block);
        _baseBlocksNode->placeBlock(block);
        block->setRendering(false);
        _renderQueue.erase(0);
    }
}

void WorldRenderer::processEffects()
{
    // TODO: use captured screen blocks which is set by LightMapper for some mysterious reason
    for (auto y = _blockRect.getMinY(); y < _blockRect.getMaxY(); y++)
    {
        for (auto x = _blockRect.getMinX(); x < _blockRect.getMaxX(); x++)
        {
            auto block = _zone->getBlockAt((uint16_t)x, (uint16_t)y);

            if (!block)
            {
                continue;
            }

            // 0x1000811EF: Emit block particles
            auto frontItem = block->getFrontItem();
            auto emitter   = frontItem->getEmitter();

            if (!emitter)
            {
                emitter = block->getLiquidItem()->getEmitter();
            }

            if (emitter)
            {
                emitParticle(emitter, block);
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
                    if (block->isBackOpaque() || block->isFrontOpaque())
                    {
                        continue;
                    }

                    renderer = _baseBlocksNode;
                    item     = block->getBaseItem();
                    break;
                case BlockLayer::BACK:
                    if (block->isFrontOpaque())
                    {
                        continue;
                    }

                    renderer = _backBlocksNode;
                    item     = block->getBackItem();
                    mod      = block->getBackMod();
                    break;
                case BlockLayer::FRONT:
                    renderer = _frontBlocksNode;
                    item     = frontItem;
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

                // Apply glow effect
                if (item->getGlow() > 0.0F)
                {
                    auto sprite = block->getTopSpriteForLayer(layer);
                    sprite->setOpacity(random(0xF0, 0xFF));
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
            }

            // 0x100081582: Special emitters
            if (frontItem->getSpecialPlacement() != SpecialPlacement::NONE)
            {
                switch (frontItem->getCode())
                {
                // Purifier
                case GECK_TUB:
                {
                    if (block->getFrontMod() > 0)
                    {
                        if (auto emitter = GameConfig::getMain()->getEmitterForName("sparkle up"))
                        {
                            auto position = block->getWorldPosition();
                            emitParticle(emitter, position + Vec2(BLOCK_SIZE * 0.5F, BLOCK_SIZE * 2.0F));
                            emitParticle(emitter, position + Vec2(BLOCK_SIZE * 1.5F, BLOCK_SIZE * 2.0F));
                        }
                    }
                    break;
                }
                // Composter
                case COMPOSTER_CHAMBER:
                {
                    if (block->getFrontMod() > 0)
                    {
                        if (auto emitter = GameConfig::getMain()->getEmitterForName("shadow steam"))
                        {
                            auto position = block->getWorldPosition();
                            position.y += BLOCK_SIZE * 2.5F;

                            if (auto particle = emitParticle(emitter, position))
                            {
                                particle->getPhysical()->setVelocity(Vec2::UNIT_Y * BLOCK_SIZE);
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
                }
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
    auto player    = Player::getMain();
    Point position = player->getPosition();
    position.y += BLOCK_SIZE * 0.8F;
    auto& winSize   = _director->getWinSize();
    auto origin     = _director->getVisibleOrigin();
    auto cameraSize = (winSize * 0.5F - origin) / _worldScale;  // Distance between camera center and screen edge
    auto minX       = cameraSize.width;
    auto maxX       = _zone->getBlocksWidth() * BLOCK_SIZE - cameraSize.width;
    auto minY       = -_zone->getBlocksHeight() * BLOCK_SIZE + cameraSize.height;
    auto maxY       = -cameraSize.height;
    auto clampedX   = MAX(minX, MIN(position.x, maxX));
    auto clampedY   = MIN(maxY, MAX(position.y, minY));
    auto distance   = math_util::getDistance(clampedX, clampedY, _cameraPosition.x, _cameraPosition.y);

    if (deltaTime < 1.0F && distance < BLOCK_SIZE * 5.0F)
    {
        MathUtil::smooth(&_cameraPosition.x, clampedX, deltaTime, position.x == clampedX ? 0.01F : 0.0F);
        MathUtil::smooth(&_cameraPosition.y, clampedY, deltaTime, position.y == clampedY ? 0.01F : 0.0F);
    }
    else
    {
        _cameraPosition.x = clampedX;
        _cameraPosition.y = clampedY;
    }

    auto viewport = _cameraPosition * _worldScale - winSize * 0.5F;
    _foreground->setPosition(-viewport);
    _foreground->setScale(_worldScale);
    _sky->setViewPosition(_cameraPosition);
    _sky->setViewScale(_worldScale);
    _cavern->setViewPosition(_cameraPosition);
    _cavern->setViewScale(_worldScale);
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

bool WorldRenderer::hasRenderedAllPlacedBlocks() const
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
            if (entity->isHuman())
            {
                _animatedCharactersNode->addChild(entity);
            }
            else
            {
                _animatedEntitiesNode->addChild(entity);
            }
        }
    }
    else if (entity->getTexture() == _entitiesNode->getTexture())
    {
        _entitiesNode->addChild(entity);
    }

    return entity;
}

Label* WorldRenderer::emote(const std::string& text, const Point& position, const Color3B& color, bool quick)
{
    auto label = MultiLabel::createWithBMFont("console.fnt", text);
    label->setScale(0.8F);
    label->setColor(color);
    label->setOpacity(222);
    label->setPosition(position);
    _textNode->addChild(label);
    auto duration  = quick ? 1.5F : 3.0;
    auto delayTime = DelayTime::create(duration - 0.5F);
    auto fadeOut   = FadeOut::create(0.5F);
    auto callFunc  = CallFuncN::create(&Node::removeFromParent);
    auto sequence  = Sequence::create({delayTime, fadeOut, callFunc});
    auto moveBy    = MoveBy::create(duration, Vec2::UNIT_Y * (quick ? 25.0F : 40.0F));
    label->runAction(Spawn::createWithTwoActions(moveBy, sequence));
    return label;
}

void WorldRenderer::generateEffect(const std::string& name, ssize_t quantity, const Point& position)
{
    auto playerPos = Player::getMain()->getPosition();
    auto distance  = math_util::getDistance(position.x, position.y, playerPos.x, playerPos.y) / BLOCK_SIZE;
    auto visible   = _blockRect.containsPoint(_zone->getBlockPointAtNodePoint(position));
    auto config    = GameConfig::getMain();

    // 0x1000835B4: Handle emitter
    if (auto emitter = config->getEmitterForName(name))
    {
        if (visible)
        {
            for (ssize_t i = 0; i < quantity; i++)
            {
                emitParticle(emitter, position);
            }
        }
        else  // Play sound effect (if it has one) even if not on screen
        {
            auto& sound = emitter->getSound();

            if (!sound.empty())
            {
                if (emitter->shouldLocalizeSound())
                {
                    AudioManager::getInstance()->playSfx(sound, position);
                }
                else
                {
                    AudioManager::getInstance()->playSfx(sound);
                }
            }
        }

        return;
    }

    // 0x100083625: Handle earthquake
    if (name == "earthquake")
    {
        _earthquake          = quantity;
        _earthquakeEpicenter = position;
        return;
    }
     
    // TODO: levelup

    // 0x1000837C3: Handle bomb effect
    if (name.starts_with("bomb") && distance < 100.0F)
    {
        struct Bomb
        {
            std::string sound;
            std::string animation;
            std::string emitter;
            ssize_t frameCount;
            float screenShake;
            float emission;
        } bomb{};

        // 0x100083804: Determine effect properties based on explosion type
        if (name.ends_with("stomp"))
        {
            bomb = {"ExplosionPip", "", "explosion cloud small", 6, clampf(quantity * 0.2F + 0.4F, 0.0F, 1.0F), 0.15F};
        }
        else if (name.ends_with("fire"))
        {
            bomb = {"ExplosionIncendiary", "boom-incendiary", "explosion cloud small", 6, 0.7F, 0.65F};
        }
        else if (name.ends_with("acid"))
        {
            bomb = {"ExplosionIncendiary", "boom-acid", "explosion cloud steam", 5, 0.7F, 0.25F};
        }
        else if (name.ends_with("frost"))
        {
            bomb = {"ExplosionPip", "boom-frost", "explosion cloud steam", 6, 0.7F, 0.3F};
        }
        else if (name.ends_with("electric"))
        {
            bomb = {"electrical_burst_01", "boom-electric", "explosion cloud small", 6, 0.7F, 0.3F};
        }
        else if (name.ends_with("teleport"))
        {
            bomb = {"electrical_fuse_burst_08", "boom-teleport", "explosion cloud small", 7, 0.45F, 0.0F};
        }
        else
        {
            bomb = {"ExplosionIncendiary", "boom", "explosion cloud small", 7, 1.0F, 1.0F};
        }

        if (visible)
        {
            // 0x10008399D: Emit explosion particles
            if (bomb.emission > 0.0F)
            {
                if (auto emitter = config->getEmitterForName(bomb.emitter))
                {
                    auto cloudCount = (int)random(quantity * 2.0F, quantity * 3.5F) * bomb.emission;

                    if (cloudCount > 0.0F)
                    {
                        auto spread = quantity * bomb.emission / 3.0F;

                        for (auto i = 0; i < cloudCount; i++)
                        {
                            if (auto particle = emitParticle(emitter, position))
                            {
                                auto physical = particle->getPhysical();
                                physical->setPosition(position.lerp(physical->getPosition(), spread));
                                particle->setScale(particle->getScale() * math_util::lerp(0.5F, 1.0F, spread));
                            }
                        }
                    }
                }
            }

            if (bomb.emission > 0.25F)
            {
                if (auto emitter = config->getEmitterForName("explosion cloud large"))
                {
                    auto cloudCount = (int)random(quantity * 0.3F, quantity * 0.6F) * bomb.emission;

                    if (cloudCount > 0.0F)
                    {
                        for (auto i = 0; i < cloudCount; i++)
                        {
                            emitParticle(emitter, position);
                        }
                    }
                }
            }

            // 0x100083C5B: Create explosion animation
            if (!bomb.animation.empty())
            {
                auto sprite = Sprite::createWithSpriteFrameName(std::format("explosions/{}-1", bomb.animation));
                sprite->setScale(random(quantity * 0.4F, quantity * 0.5F));
                Vector<SpriteFrame*> frames;
                frames.reserve(bomb.frameCount);

                for (auto i = 0; i < bomb.frameCount; i++)
                {
                    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(
                        std::format("explosions/{}-{}", bomb.animation, i + 1));

                    if (frame)
                    {
                        frames.pushBack(frame);
                    }
                }

                auto animate  = Animate::create(Animation::createWithSpriteFrames(frames, 0.08F));
                auto scaleBy  = ScaleBy::create(0.3F, quantity * 0.15F);
                auto spawn    = Spawn::createWithTwoActions(animate, scaleBy);
                auto callFunc = CallFuncN::create(&Node::removeFromParent);
                auto sequence = Sequence::createWithTwoActions(spawn, callFunc);
                sprite->runAction(sequence);
                sprite->setPosition(position);
                sprite->setRotation(random(0.0F, 360.0F));
                _effectsNode->addChild(sprite);
                auto flash = math_util::lerp(1.0F, 0.0F, distance / 20.0F);
                _lightmapper->flash(flash);
            }
        }

        // 0x100083EF8: Update screen shake value
        auto shake = bomb.screenShake * math_util::lerp(quantity * 0.2F, 0.0F, distance / 20.0F);
        _explosion = MAX(_explosion, shake);

        // 0x100083F51: Automatically determine sound to use based on quantity and distance
        std::string sound = bomb.sound;  // Copy

        if (sound.empty())
        {
            sound = distance >= 30.0F ? "ExplosionDistant" : quantity >= 6 ? "ExplosionPop" : "ExplosionPip";
        }

        AudioManager::getInstance()->playSfx(sound, position, 1.0F, 2.0F);
    }
}

Action* WorldRenderer::generateMiningCracks(BaseBlock* block, BlockLayer layer, float duration)
{
    auto animate = Animate::create(_miningCracksAnimation);
    animate->setDuration(duration);
    auto callFunc = CallFunc::create([=]() { block->completeMining(layer); });
    auto sequence = Sequence::createWithTwoActions(animate, callFunc);
    auto sprite   = Sprite::createWithSpriteFrameName("crack-1");
    math_util::scaleToSize(sprite, Vec2::ONE * BLOCK_SIZE);
    sprite->setPosition(_zone->getPointAtBlock(block->getX(), block->getY()));
    _effectsNode->addChild(sprite);
    sprite->runAction(sequence);
    return sequence;
}

void WorldRenderer::generateMiningDebris(BaseBlock* block, BlockLayer layer, bool ineffectual)
{
    if (block && utils::gettime() > _nextDebrisAt)
    {
        _nextDebrisAt = utils::gettime() + BLOCK_DEBRIS_INTERVAL;
        generateBlockDebris(block, layer, ineffectual, random(2, 4));
    }
}

void WorldRenderer::generateBlockDebris(BaseBlock* block, BlockLayer layer, bool ineffectual, ssize_t count)
{
    if (_freeDebrisIndex >= DEBRIS_POOL_SIZE)
    {
        return;  // Debris pool empty, skip
    }

    if (auto sprite = block->getMainSpriteForLayer(layer))
    {
        // This check serves as a replacement for the layerRendererForItem function
        // TODO: there should be a less vague way to determine if a block should emit mining debris
        auto renderer = static_cast<WorldLayerRenderer*>(sprite->getUserData());
        auto texture  = renderer->getBatchNode()->getTexture();

        if (texture != _baseBlocksNode->getBatchNode()->getTexture() &&
            texture != _backBlocksNode->getBatchNode()->getTexture() &&
            texture != _fronterBlocksNode->getBatchNode()->getTexture() &&
            texture != _fronterBiomeBlocksNode->getBatchNode()->getTexture())
        {
            return;
        }

        // Determine position & velocity
        auto playerPos = Player::getMain()->getPosition();
        auto blockPos  = block->getWorldPosition();
        auto direction = (blockPos - playerPos).getNormalized();
        auto velocity  = direction * random(1000.0F, 2500.0F);
        velocity       = velocity * random(-0.1F, 0.1F) + Vec2(velocity.y, -velocity.x) * random(-0.1F, 0.1F);
        auto offsetX   = rand_minus1_1() * 0.5F * BLOCK_SIZE;
        auto offsetY   = rand_minus1_1() * 0.5F * BLOCK_SIZE;
        auto position  = blockPos + Vec2(offsetX, offsetY);

        // Spawn particles
        for (ssize_t i = 0; i < count; i++)
        {
            if (_freeDebrisIndex >= DEBRIS_POOL_SIZE)
            {
                break;  // Debris pool empty, stop
            }

            auto debris = _debrisPool[_freeDebrisIndex++];
            auto angle  = MATH_DEG_TO_RAD(random(0.0F, 360.0F));

            if (!ineffectual)
            {
                debris->spawnForSprite(sprite, position, velocity);
                debris->getPhysical()->getBody()->setAngle(angle);
            }
            else
            {
                if (auto emitter = GameConfig::getMain()->getEmitterForName("steam"))
                {
                    debris->spawnParticle(emitter, position);
                    auto body = debris->getPhysical()->getBody();
                    body->setVelocity(velocity * 0.5F);
                    body->setAngle(angle);
                    debris->setColor(color_util::rgbToColor(0x5A3C1E));
                    debris->setScale(debris->getScale() * 0.5F);
                    _nextDebrisAt += 0.05F;
                }
            }
        }
    }
}

Debris* WorldRenderer::emitParticle(Emitter* emitter, BaseBlock* block)
{
    auto point = block->getWorldPosition();
    point.x += (rand_0_1() - 0.5F) * BLOCK_SIZE;
    point.y += (rand_0_1() - 0.5F) * BLOCK_SIZE;
    return emitParticle(emitter, point);
}

Debris* WorldRenderer::emitParticle(Emitter* emitter, const Point& point)
{
    return emitParticle(emitter, point, emitter->getFrequency());
}

Debris* WorldRenderer::emitParticle(Emitter* emitter, const Point& point, float frequency)
{
    if (rand_0_1() >= frequency)
    {
        return nullptr;
    }

    Debris* debris = nullptr;
    AX_ASSERT(_freeDebrisIndex >= 0);

    if (_freeDebrisIndex < DEBRIS_POOL_SIZE)
    {
        debris      = _debrisPool[_freeDebrisIndex++];
        Vec2 offset = emitter->getPositionRange();  // Copy
        offset.x *= rand_minus1_1() * 0.5F * BLOCK_SIZE;
        offset.y *= rand_minus1_1() * 0.5F * BLOCK_SIZE;
        debris->spawnParticle(emitter, point + offset);
    }

    auto& sound = emitter->getSound();

    if (!sound.empty())
    {
        if (emitter->shouldLocalizeSound())
        {
            AudioManager::getInstance()->playSfx(sound, point);
        }
        else
        {
            AudioManager::getInstance()->playSfx(sound);
        }
    }

    return debris;
}

void WorldRenderer::recycleDebris(Debris* debris)
{
    if (debris->isActive())
    {
        AX_ASSERT(_freeDebrisIndex > 0);
        _freeDebrisIndex--;
        auto src = debris->getPoolIndex();

        if (src != _freeDebrisIndex)
        {
            auto free = _debrisPool[_freeDebrisIndex];
            _debrisPool[src] = free;
            free->setPoolIndex(src);
            _debrisPool[_freeDebrisIndex] = debris;
            debris->setPoolIndex(_freeDebrisIndex);
        }
    }
}

void WorldRenderer::emitItemAnimation(Item* item, const Point& position, ssize_t count)
{
    if (_zone->getState() != WorldZone::State::ACTIVE)
    {
        return; 
    }

    if (auto frame = item->getInventoryFrame())
    {
        for (ssize_t i = 0; i < count; i++)
        {
            auto sprite = Sprite::createWithSpriteFrame(frame);
            sprite->setPosition(position);
            sprite->setScale(0.5F);
            // FIXME: Needs to use ONE_MINUS_CONSTANT_COLOR which isn't supported
            sprite->setBlendFunc({backend::BlendFactor::ONE, backend::BlendFactor::ONE_MINUS_SRC_COLOR});
            sprite->setColor(item->getSpriteColor());  // BUGFIX: Crystal block colors
            _guiNode->addChild(sprite);
            auto moveBy = MoveBy::create(0.5F, Vec2::UNIT_Y * BLOCK_SIZE * 1.1F);

            if (i > 0)
            {
                // Use action for delay so we don't have to use a scheduler
                sprite->setOpacity(0);
                auto delayTime = DelayTime::create(i * 0.075F);
                auto fadeIn    = FadeIn::create(0.0F);
                auto fadeOut   = FadeOut::create(0.5F);
                auto callFunc  = CallFuncN::create(&Node::removeFromParent);
                auto sequence  = Sequence::createWithTwoActions(fadeOut, callFunc);
                auto spawn     = Spawn::createWithTwoActions(moveBy, sequence);
                sprite->runAction(Sequence::create({delayTime, fadeIn, spawn}));
            }
            else
            {
                ax_util::fadeOutAndRemove(sprite);
                sprite->runAction(moveBy);
            }
        }
    }
}

Point WorldRenderer::getNodePointForScreenPoint(const Point& point) const
{
    return _foreground->convertToNodeSpace(point);
}

Point WorldRenderer::getScreenPointForNodePoint(const Point& point) const
{
    return _foreground->convertToWorldSpace(point);
}

}  // namespace opendw

#include "WorldLayerRenderer.h"

#include "graphics/backend/MaskedSprite.h"
#include "graphics/backend/MaskedSpriteBatchNode.h"
#include "graphics/WorldRenderer.h"
#include "util/MapUtil.h"
#include "util/MathUtil.h"
#include "zone/BaseBlock.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"
#include "GameConfig.h"
#include "GameManager.h"

#define BASE_EARTH          2
#define BASE_MAW            5
#define BASE_PIPE           6
#define PLUGGED_MAW         7
#define PLUGGED_PIPE        8
#define WATER               192
#define DAGUERREOTYPE_SMALL 754
#define DAGUERREOTYPE_LARGE 755
#define GIANT_CLOCK         761
#define LANDSCAPE           797
#define WINE_PRESS          863
#define HELL_DISH           1010

USING_NS_AX;

namespace opendw
{

WorldLayerRenderer::~WorldLayerRenderer()
{
    if (_batchNode)
    {
        sTotalSpriteCount -= _batchNode->getChildrenCount();
    }
}

WorldLayerRenderer* WorldLayerRenderer::createWithLayer(BlockLayer layer, MaskedSpriteBatchNode* batchNode)
{
    CREATE_INIT(WorldLayerRenderer, initWithLayer, layer, batchNode);
}

bool WorldLayerRenderer::initWithLayer(BlockLayer layer, MaskedSpriteBatchNode* batchNode)
{
    if (!Node::init())
    {
        return false;
    }

    _layer                 = layer;
    _batchNode             = batchNode;
    _zone                  = GameManager::getInstance()->getZone();
    _placeBackgroundsInAlt = false;
    _biomeRenderer         = false;
    auto cache             = SpriteFrameCache::getInstance();
    _shadowShallowFrame    = cache->getSpriteFrameByName("borders/whole-shadow");
    _shadowDeepFrame       = cache->getSpriteFrameByName("borders/earth-deep");
    _shadowDeepSideMask    = cache->getSpriteFrameByName("masks/earth-deep-side")->getRect();
    _zeroMask              = cache->getSpriteFrameByName("masks/opaque")->getRect();
    addChild(batchNode);
    return true;
}

void WorldLayerRenderer::clear()
{
    if (_biomeRenderer)
    {
        _recycledSprites.clear();
        sTotalSpriteCount -= _batchNode->getChildrenCount();
        _batchNode->removeAllChildren();
        return;
    }

    // NOTE: we can (safely) assume that sprites are already recycled by the chunk recycler
    /* for (auto& child : _batchNode->getChildren())
    {
        auto sprite = dynamic_cast<MaskedSprite*>(child);

        if (sprite)
        {
            recycleSprite(sprite);
        }
    }*/
}

void WorldLayerRenderer::placeSpecialItem(BaseBlock* block, Item* item)
{
    auto config = GameManager::getInstance()->getConfig();

    // TODO: crest, machine
    switch (item->getSpecialPlacement())
    {
    // 0x1000A9075: Framed
    case SpecialPlacement::FRAMED:
    {
        // Place frame sprites
        // TODO: not a 100% accurate implementation
        auto& sprites = map_util::getArray(item->getData(), "sprites");
        auto count    = MIN(4, sprites.size());

        for (auto i = 0; i < count; i++)
        {
            auto name  = map_util::getString(sprites[i].asValueMap(), "frames");
            auto frame = config->getCurrentBiomeFrame(name);
            placeSprite(block, nullptr, frame, false, true, ModType::ROTATION, i, 10);
        }

        // 0x1000A93C3: Place landscape damage sprite
        if (item->getCode() == LANDSCAPE)
        {
            auto frame    = config->getCurrentBiomeFrame("furniture/painting-distress");
            auto random   = block->getX() + block->getY();
            auto rotation = random % 4;
            auto opacity  = (random % 3 + 150) & 0xFF;
            auto sprite   = placeSprite(block, nullptr, frame, false, true, ModType::ROTATION, rotation, 9);
            sprite->setOpacity(opacity);
        }

        break;
    }
    // 0x1000A99C9: Clock
    case SpecialPlacement::CLOCK:
    {
        // Place hand sprites and rotate them based on the current time in the zone
        struct ClockHand
        {
            std::string suffix;
            float duration;
            float rotation;
            int z;
        };

        auto daytime      = _zone->getDayTime();
        ClockHand hands[] = {ClockHand("minute", 50.0F, fmodf(daytime, 1.0F / 24.0F) * 24.0F * 360.0F, 9),
                             ClockHand("hour", 600.0F, fmodf(daytime, 0.5F) * 2.0F * 360.0F, 10)};

        for (auto& hand : hands)
        {
            auto name   = std::format("{}-{}-hand", item->getName(), hand.suffix);
            auto frame  = config->getCurrentBiomeFrame(name);
            auto action = RepeatForever::create(RotateBy::create(hand.duration, 360.0F));
            auto sprite = placeSprite(block, nullptr, frame, false, true, ModType::NONE, 0, hand.z);
            sprite->setAnchorPoint({0.5F, 0.1F});
            sprite->setRotation(hand.rotation);
            sprite->setPositionY(sprite->getPositionY() + sprite->getContentSize().height * -0.4F);
            sprite->runAction(action);
            sprite->setTag(ACTION_SPRITE_TAG);
        }

        // 0x1000A9CAD: Place gears & front cover for giant clock
        if (item->getCode() == GIANT_CLOCK)
        {
            // Place gears
            float directions[] = {1.0F, -1.0F};

            for (auto i = 0; i < 2; i++)
            {
                auto name   = std::format("furniture/clock-giant-gear-large-{}", i + 1);
                auto frame  = config->getCurrentBiomeFrame(name);
                auto action = RepeatForever::create(RotateBy::create(0.83334F, 360.0F * directions[i]));
                auto sprite = placeSprite(block, nullptr, frame, false, true, ModType::NONE, 0, 5);
                sprite->runAction(action);
                sprite->setTag(ACTION_SPRITE_TAG);
            }

            // Place front cover
            auto frame = config->getCurrentBiomeFrame("furniture/clock-giant-front");
            placeSprite(block, nullptr, frame, false, true, ModType::NONE, 0, 6);
        }

        break;
    }
    // 0x1000AA27E: Unique
    case SpecialPlacement::UNIQUE:
        placeUniqueItem(block, item);
        break;
    }
}

void WorldLayerRenderer::placeUniqueItem(BaseBlock* block, Item* item)
{
    auto config = GameManager::getInstance()->getConfig();
    auto x      = block->getX();
    auto y      = block->getY();

    // TODO: finish, add all other unique items
    // Add your cool custom block animations (or other unique behavior) here :)
    switch (item->getCode())
    {
    // 0x1000AAFA4: Small daguerreotype
    case DAGUERREOTYPE_SMALL:
    {
        auto type  = (x + y) % 12;
        auto name  = std::format("furniture/daguerreotype-image-{}", type + 1);
        auto frame = config->getCurrentBiomeFrame(name);
        auto sprite = placeSprite(block, nullptr, frame, false, true, ModType::NONE, 0, 3);
        sprite->setScale(0.6F);
        sprite->setFlippedX(~x & 1);
        sprite->setPosition(sprite->getPosition() - Vec2::ONE * BLOCK_SIZE * 0.5F);
        break;
    }
    // 0x1000AA42E: Large daguerreotype
    case DAGUERREOTYPE_LARGE:
    {
        struct Portrait
        {
            int type;
            float offset;
            bool flipped;
            int z;
        };

        // TODO: better randomness for type
        Portrait portraits[] = {Portrait((x + y) % 12, BLOCK_SIZE * -0.25F, ~y & 1, 4),
                                Portrait((x * y) % 12, BLOCK_SIZE * 0.25F, ~x & 1, 3)};

        for (auto& portrait : portraits)
        {
            auto name      = std::format("furniture/daguerreotype-image-{}", portrait.type + 1);
            auto frame     = config->getCurrentBiomeFrame(name);
            auto sprite    = placeSprite(block, nullptr, frame, false, true, ModType::NONE, 0, portrait.z);
            auto& position = sprite->getPosition();
            sprite->setFlippedX(portrait.flipped);
            sprite->setPositionX(position.x - portrait.offset);
        }

        break;
    }
    // 0x1000AA8C8: Wine press
    case WINE_PRESS:
    {
        if (block->getFrontMod() > 0)
        {
            auto frame    = config->getCurrentBiomeFrame("mechanical/winepress-piston");
            auto sprite   = placeSprite(block, nullptr, frame, false, true);
            auto distance = BLOCK_SIZE * 0.7F;
            auto moveDown = MoveBy::create(0.15F, Vec2::UNIT_Y * -distance);
            auto moveUp   = MoveBy::create(1.0F, Vec2::UNIT_Y * distance);
            auto sequence = Sequence::createWithTwoActions(moveDown, moveUp);
            sprite->runAction(RepeatForever::create(sequence));
            sprite->setTag(ACTION_SPRITE_TAG);
        }

        break;
    }
    // 0x1000AA340: Infernal protector
    case HELL_DISH:
    {
        auto frame  = config->getCurrentBiomeFrame("hell/dish-spinner");
        auto sprite = placeSprite(block, nullptr, frame, false, true, ModType::NONE, 0, 5);
        auto action = RepeatForever::create(RotateBy::create(0.25F, 360.0F));
        sprite->runAction(action);
        sprite->setTag(ACTION_SPRITE_TAG);
        break;
    }
    }
}

void WorldLayerRenderer::placeBlock(BaseBlock* block)
{
    block->recycleSprites(_layer);
    Item* item   = nullptr;
    uint16_t mod = 0;

    switch (_layer)
    {
    case BlockLayer::BASE:
    {
        item      = block->getBaseItem();
        auto code = item->getCode();

        // 0x1000A3D13: Place plugs on plugged spawners
        if (code == PLUGGED_MAW)
        {
            auto frame  = GameManager::getInstance()->getConfig()->getCurrentBiomeFrame("base/maw-plug");
            auto sprite = placeSprite(block, nullptr, frame, false, true, ModType::NONE, 0, 11);
            sprite->setAnchorPoint({0.5F, 0.7F});
        }
        else if (code == PLUGGED_PIPE)
        {
            auto frame  = GameManager::getInstance()->getConfig()->getCurrentBiomeFrame("base/pipe-plug");
            auto sprite = placeSprite(block, nullptr, frame, false, true, ModType::NONE, 0, 11);
            sprite->setAnchorPoint({0.5F, 0.65F});
        }
        else if (code != BASE_MAW && code != BASE_PIPE && (block->isBackOpaque() || block->isFrontOpaque()))
        {
            // Don't draw block if it is behind an opaque block unless it is a maw or pipe
            return;
        }

        break;
    }
    case BlockLayer::BACK:
        if (block->isFrontOpaque())
        {
            return;
        }

        item = block->getBackItem();
        mod  = block->getBackMod();
        break;
    case BlockLayer::FRONT:
        item = block->getFrontItem();
        mod  = block->getFrontMod();

        // 0x1000A3B5F: Place special item if necessary
        if (item->getSpecialPlacement() != SpecialPlacement::NONE)
        {
            placeSpecialItem(block, item);
        }

        break;
    case BlockLayer::LIQUID:
        item = block->getLiquidItem();
        mod  = block->getLiquidMod();

        if (mod == 0)
        {
            return;
        }

        break;
    }

    if (item)
    {
        if (item->isVisible() && item->getCode() > 0)
        {
            placeItem(block, item, mod);
        }
    }
    else
    {
        AXLOGW("[WorldLayerRenderer] {}: Null item at block {}, {}", _name, block->getX(), block->getY());
    }
}

void WorldLayerRenderer::placeItem(BaseBlock* block, Item* item, uint8_t mod)
{
    auto x             = block->getX();
    auto y             = block->getY();
    auto wholeness     = block->getWholeness();
    auto continuity    = block->getContinuityForLayer(_layer);
    auto spriteZ       = item->getSpriteZ();
    auto tileable      = item->isTileable();
    auto modType       = item->getModType();
    bool center        = item->isCentered();
    auto worldRenderer = _zone->getWorldRenderer();
    auto config        = GameManager::getInstance()->getConfig();
    auto spriteIndex   = x + y;

    // Place background sprite
    auto background         = item->getBackground();
    auto& backgroundOptions = item->getBackgroundOptions();

    if (!backgroundOptions.empty())
    {
        auto count = backgroundOptions.size();

        switch (modType)
        {
        case ModType::SPRITE:
            background = backgroundOptions[mod % count];
            break;
        case ModType::STACK:
            background = backgroundOptions[MAX(0, MIN(mod / 5, count - 1))];
            break;
        default:
            background = backgroundOptions[spriteIndex % count];
            break;
        }
    }

    if (background)
    {
        placeSprite(block, nullptr, background, _layer == BlockLayer::BASE || center || tileable, true);
    }

    // Place continuity animation sprite (independent of sprite continuity)
    auto& continuityAnimation = item->getSpriteContinuityAnimation();

    if (!continuityAnimation.empty())
    {
        auto& spriteInfo = continuityAnimation[continuity & 0xF].back();
        auto frame       = spriteInfo.options[0];
        auto rotation    = spriteInfo.rotation;
        auto sprite      = placeSprite(block, nullptr, frame, true, true, ModType::ROTATION_DEGREES, rotation,
                                       item->getSpriteZ() + 1);  // HACK: always render on top
        sprite->setOpacity(item->getSpriteContinuityAnimationOpacity());
        sprite->setTag(ANIMATED_SPRITE_TAG - 3 - (static_cast<uint8_t>(_layer) - 1));
    }

    // Place foreground sprite
    // Priority order = animation -> continuity -> sprite
    auto foreground        = item->getSpriteFrame();
    auto& spriteAnimation  = item->getSpriteAnimation();
    auto& spriteContinuity = item->getSpriteContinuity();

    if (!spriteAnimation.empty())
    {
        auto sprite = placeSprite(block, nullptr, spriteAnimation[0], false, true, modType, mod, 2);
        sprite->setTag(ANIMATED_SPRITE_TAG - (static_cast<uint8_t>(_layer) - 1));
        sprite->setColor(item->getSpriteAnimationColor());
    }
    else if (!spriteContinuity.empty())
    {
        auto& data = spriteContinuity[block->getContinuityForLayer(_layer) & 0xF].back();
        auto frame = data.frame;

        if (frame)
        {
            auto sprite = placeSprite(block, nullptr, frame, true, true, ModType::ROTATION_DEGREES, data.rotation,
                                      MAX(1, spriteZ));
            sprite->setFlippedX(data.flipX);
            sprite->setFlippedY(data.flipY);
        }
    }
    else
    {
        // Use sprite options if they're present, otherwise just use single sprite
        auto& spriteOptions = item->getSpriteOptions();

        if (!spriteOptions.empty())
        {
            auto count = spriteOptions.size();

            switch (modType)
            {
            case ModType::SPRITE:
                foreground = spriteOptions[mod % count];
                break;
            case ModType::STACK:
                foreground = spriteOptions[MAX(0, MIN(mod / 5, count - 1))];
                break;
            default:
                foreground = spriteOptions[spriteIndex % count];
                break;
            }
        }

        if (foreground)
        {
            auto sprite = placeSprite(block, nullptr, foreground, tileable, !center, modType, mod, MAX(1, spriteZ));
            sprite->setColor(item->getSpriteColor());

            // Set mask frame if present
            auto maskFrame = item->getMaskFrame();

            if (maskFrame)
            {
                sprite->setMaskRect(maskFrame->getRect());
            }

            // 0x1000A72F9: Make sprite slightly smaller if it is framed
            if (item->getSpecialPlacement() == SpecialPlacement::FRAMED)
            {
                sprite->setScale(0.875F);
            }

            // 0x1000A732B: Create jiggle animation if necessary
            auto jiggle = item->getJiggle();

            if (jiggle > 0.0F)
            {
                auto distance = 0.05F * jiggle * BLOCK_SIZE;
                auto moveUp   = MoveBy::create(0.1F, Vec2::UNIT_Y * distance);
                auto moveDown = MoveBy::create(0.1F, Vec2::UNIT_Y * -distance);
                auto sequence = Sequence::createWithTwoActions(moveUp, moveDown);
                auto repeat   = Repeat::create(sequence, 3);
                auto delay    = DelayTime::create(3.0F);
                sprite->runAction(RepeatForever::create(Sequence::createWithTwoActions(repeat, delay)));
                sprite->setTag(ACTION_SPRITE_TAG);
            }

            // 0x1000A4B16: Apply decay if necessary
            if (modType == ModType::DECAY && mod > 0)
            {
                auto decayIndex = y + x + ((y + 1) * (x + 1)) / (y + x);

                if (mod < 2)
                {
                    // 0x1000A4BF0: Light decay
                    auto& options = config->getSingleDecayForMaterial(item->getMaterial());

                    if (!options.empty())
                    {
                        auto frame = options[decayIndex % options.size()];
                        placeSprite(block, nullptr, frame, false, false);
                    }
                }
                else
                {
                    // 0x1000A65D1: Heavy decay (holes in blocks)
                    auto& masks = config->getSingleDecayMasks();

                    if (!masks.empty())
                    {
                        auto& mask = masks[decayIndex % masks.size()];

                        if (!mask.empty())
                        {
                            auto& name = mask[MAX(0, MIN(mask.size() - 1, mod - 2))];
                            sprite->setMaskFrame(std::format("masks/{}", name));
                            auto accent       = config->getCurrentBiomeFrame(std::format("mask_borders/{}", name));
                            auto accentSprite = placeSprite(block, nullptr, accent, false, true, ModType::NONE, 0, 3);
                            accentSprite->setOpacity(0xC0);
                        }
                    }
                }
            }
        }
    }

    // 0x1000A4701: Place border sprites
    auto& borderContinuity = item->getBorderContinuity();

    if (!borderContinuity.empty())
    {
        auto& color     = item->getBorderColor();
        auto& border    = borderContinuity[~continuity & 0xF];
        auto& corner    = borderContinuity[16];
        auto& rotations = worldRenderer->getContinuityCornerMasks()[continuity];
        placeBorder(block, border, color);
        placeCorners(block, corner, rotations, color);
    }

    // 0x1000A47FE: Place external border sprites
    auto& borderExternal = item->getBorderContinuityExternal();

    if (!borderExternal.empty())
    {
        auto mask   = borderExternal[spriteIndex % borderExternal.size()];
        auto target = background ? background : foreground;  // Prioritize background if it is present

        // 0x1000A4DF9: Top
        if (!(continuity & BaseBlock::CONTINUITY_TOP))
        {
            auto above = block->getAbove();

            if (!(wholeness & BaseBlock::CONTINUITY_TOP) || (above && above->getFrontItem()->getSpriteZ() < spriteZ))
            {
                auto sprite = getNextSprite(mask->getRect(), MaskOrientation::UP);
                placeSprite(block, sprite, target, true, false, ModType::POSITION, 3, spriteZ);
            }
        }

        // 0x1000A4F7D: Right
        if (!(continuity & BaseBlock::CONTINUITY_RIGHT))
        {
            auto right = block->getRight();

            if (!(wholeness & BaseBlock::CONTINUITY_RIGHT) || (right && right->getFrontItem()->getSpriteZ() < spriteZ))
            {
                auto sprite = getNextSprite(mask->getRect(), MaskOrientation::RIGHT);
                placeSprite(block, sprite, target, true, false, ModType::POSITION, 2, spriteZ);
            }
        }

        // 0x1000A50F7: Bottom
        if (!(continuity & BaseBlock::CONTINUITY_BOTTOM))
        {
            auto below = block->getBelow();

            if (!(wholeness & BaseBlock::CONTINUITY_BOTTOM) || (below && below->getFrontItem()->getSpriteZ() < spriteZ))
            {
                auto sprite = getNextSprite(mask->getRect(), MaskOrientation::DOWN);
                placeSprite(block, sprite, target, true, false, ModType::POSITION, 1, spriteZ);
            }
        }

        // 0x1000A5273: Left
        if (!(continuity & BaseBlock::CONTINUITY_LEFT))
        {
            auto left = block->getLeft();

            if (!(wholeness & BaseBlock::CONTINUITY_LEFT) || (left && left->getFrontItem()->getSpriteZ() < spriteZ))
            {
                auto sprite = getNextSprite(mask->getRect(), MaskOrientation::LEFT);
                placeSprite(block, sprite, target, true, false, ModType::POSITION, 4, spriteZ);
            }
        }
    }

    // 0x1000A46F2: Place drop shadows if necessary
    if (_layer == BlockLayer::FRONT)
    {
        // Shadow placed underneath the borders of certain blocks
        if (item->hasBorderShadow() && !(continuity & BaseBlock::CONTINUITY_TOP))
        {
            auto shadow = placeSprite(block, nullptr, _shadowShallowFrame, true, true, ModType::NONE, 0, 5);
            shadow->setOpacity(0x8C);
        }
    }
    else if (_layer == BlockLayer::BASE || _layer == BlockLayer::BACK)
    {
        auto fronterItem = _layer == BlockLayer::BASE ? block->getBackItem() : block->getFrontItem();

        if (!fronterItem->isOpaque() && item->isOpaque())
        {
            auto accentsNode = worldRenderer->getBackAccentsNode();

            if (wholeness > 0)
            {
                // 0x1000A634D: Border wholeness
                // NOTE: it explicitly uses the border wholeness of base/earth
                // auto& borderWholeness = item->getBorderWholeness();
                auto& borderWholeness = config->getItemForCode(BASE_EARTH)->getBorderWholeness();

                if (!borderWholeness.empty())
                {
                    auto& border    = borderWholeness[wholeness & 0xF];
                    auto& corner    = borderWholeness[16];
                    auto& rotations = worldRenderer->getWholenessCornerMasks()[wholeness];
                    accentsNode->placeBorder(block, border, Color3B::WHITE, 0x50);
                    accentsNode->placeCorners(block, corner, rotations, Color3B::WHITE, 0x50);
                }
            }

            // 0x1000A649: Deep shadow
            auto above = block->getAbove();

            if (above)
            {
                auto topLeft  = above->getLeft();
                auto topRight = above->getRight();

                // HACK: Fix shadows for multi-width blocks (only works up to a width of 2!)
                bool wideShadow =
                    topLeft && topLeft->getFrontItem()->hasShadow() && topLeft->getFrontItem()->getWidth() > 1;

                if (above->getFrontItem()->hasShadow() || wideShadow)
                {
                    auto sprite =
                        accentsNode->placeSprite(block, nullptr, _shadowDeepFrame, true, true, ModType::NONE, 0, 5);
                    sprite->setOpacity(0x8C);

                    // Place side shadows if necessary
                    // TODO: original implementation does not use POSITION mod
                    if (topLeft && !topLeft->getFrontItem()->hasShadow())
                    {
                        auto sprite = accentsNode->getNextSprite(_shadowDeepSideMask, MaskOrientation::UP);
                        accentsNode->placeSprite(block, sprite, _shadowDeepFrame, true, true, ModType::POSITION, 4, 5);
                        sprite->setOpacity(0x8C);
                    }

                    // Only place right side shadow if above block does not extend its shadow to the right
                    if (above->getFrontItem()->getWidth() < 2 && topRight && !topRight->getFrontItem()->hasShadow())
                    {
                        auto sprite = accentsNode->getNextSprite(_shadowDeepSideMask, MaskOrientation::DOWN);
                        accentsNode->placeSprite(block, sprite, _shadowDeepFrame, true, true, ModType::POSITION, 2, 5);
                        sprite->setOpacity(0x8C);
                    }
                }
            }
        }
    }
}

void WorldLayerRenderer::placeBorder(BaseBlock* block,
                                     const Item::ContinuitySpriteList& border,
                                     const Color3B& color,
                                     uint8_t opacity)
{
    int z = 10;

    for (auto& target : border)
    {
        auto frame = target.frame;

        if (!frame)
        {
            continue;
        }

        auto rotation = target.rotation;
        auto sprite   = placeSprite(block, nullptr, frame, true, true, ModType::ROTATION_DEGREES, rotation, z++);
        sprite->setColor(color);
        sprite->setOpacity(opacity);
    }
}

void WorldLayerRenderer::placeCorners(BaseBlock* block,
                                      const Item::ContinuitySpriteList& border,
                                      const std::vector<uint16_t>& corners,
                                      const Color3B& color,
                                      uint8_t opacity)
{
    for (auto& target : border)
    {
        auto frame = target.frame;

        if (!frame)
        {
            continue;
        }

        for (auto corner : corners)
        {
            auto sprite = placeSprite(block, nullptr, frame, true, true, ModType::ROTATION_DEGREES, corner, 10);
            sprite->setColor(color);
            sprite->setOpacity(opacity);
        }
    }
}

MaskedSprite* WorldLayerRenderer::placeSprite(BaseBlock* block,
                                              MaskedSprite* sprite,
                                              SpriteFrame* frame,
                                              bool tileable,
                                              bool aligned,
                                              ModType modType,
                                              uint16_t mod,
                                              int z,
                                              bool push)
{
    // 0x1000A74E6: Substitute with depth frame if there is one
    auto& depthGraphics = _zone->getDepthGraphics();

    if (!depthGraphics.empty() && frame)
    {
        const auto& it = depthGraphics.find(std::string(frame->getName()));  // TODO

        if (it != depthGraphics.end())
        {
            auto& targets = it->second;

            // Find the depth frame that corresponds to the depth of the block
            for (ssize_t i = 0; i < targets.size(); i++)
            {
                auto depth = targets[i].first;

                if (block->getY() >= depth)
                {
                    auto next = targets[i].second;

                    if (next)
                    {
                        frame = next;
                    }
                }
            }
        }
    }

    // 0x1000A76FF: Delegate to an alt renderer if we can't (or shouldn't) render this block
    if (frame && frame->getTexture() != _batchNode->getTexture() || (z == 0 && _placeBackgroundsInAlt))
    {
        for (auto alt : _altRenderers)
        {
            if (frame->getTexture() == alt->getBatchNode()->getTexture())
            {
                // 0x1000A795F: Create a new sprite for the alt renderer if necessary
                if (sprite && sprite->getParent() != alt->getBatchNode())
                {
                    static_cast<WorldLayerRenderer*>(sprite->getParent()->getParent())->recycleSprite(sprite);
                    sprite = alt->getNextSprite(sprite->getMaskRect(), sprite->getMaskOrientation());
                }

                return alt->placeSprite(block, sprite, frame, tileable, aligned, modType, mod, z, push);
            }
        }

        AXLOGW("[WorldLayerRenderer] {}: No alt renderer found for frame {}", _name, frame->getName());
    }

    // 0x1000A7891: Create new sprite if current one is nullptr
    if (!sprite)
    {
        sprite = getNextSprite();
    }

    // 0x1000A78B5: Return if frame is nullptr
    if (!frame)
    {
        sprite->setVisible(false);
        return sprite;
    }

    auto x = block->getX();
    auto y = block->getY();

    // 0x1000A78E4: Handle position mod offset
    if (modType == ModType::POSITION && mod > 0)
    {
        auto value = ((mod - 1) % 4) + 1;
        x += ((value & 1) == 0) * (3 - value);  // 2 = x++, 4 = x--
        y += ((value & 1) == 1) * (2 - value);  // 1 = y++, 3 = y--
    }

    // At this point we kinda just freestyle because the logic gets quite complex
    Rect textureRect = frame->getRect();
    auto trimmedSize = frame->getOriginalSize() - textureRect.size;
    Vec2 offset      = frame->getOffset();
    auto origin      = trimmedSize * 0.5F;
    auto position    = _zone->getPointAtBlock(x, y);
    auto rotation    = modType == ModType::ROTATION ? (mod * 90) : modType == ModType::ROTATION_DEGREES ? mod : 0;
    auto flipX       = modType == ModType::ROTATION && mod == 4;  // TODO: doesn't take mirrorable into account
    auto modTile     = modType == ModType::TILING;

    // Handle tiling
    uint8_t tileX = 0;
    uint8_t tileY = 0;

    if (tileable || modTile)
    {
        uint8_t tilesWidth  = MAX(1.0F, ceilf(frame->getOriginalSize().width / BLOCK_SIZE));
        uint8_t tilesHeight = MAX(1.0F, ceilf(frame->getOriginalSize().height / BLOCK_SIZE));
        auto tileWidth      = frame->getOriginalSize().width / tilesWidth;
        auto tileHeight     = frame->getOriginalSize().height / tilesHeight;
        auto tileSize       = Size(tileWidth, tileHeight);

        // TODO: flip assumes that the sprite is stored vertically
        auto flip = rotation == 90 || rotation == 270;
        tileX     = modTile ? mod % tilesWidth : (flip ? y : x) % tilesWidth;
        tileY     = modTile ? (mod / tilesWidth) % tilesHeight : (flip ? x : y) % tilesHeight;
        auto tile = Point(tileX * tileWidth, tileY * tileHeight);

        // Create a fake origin to start tiling from and clamp it to the texture rect afterwards
        auto untrimmedOrigin = textureRect.origin - (origin + offset);
        auto tileRect        = Rect(untrimmedOrigin + tile, tileSize);
        Vec2 rectOffset;
        textureRect = math_util::clampRect(tileRect, textureRect, &rectOffset);

        if (modTile)
        {
            position += rectOffset * 0.5F;
        }
    }

    // 0x1000A8452: Hard coded liquid rules
    if (_layer == BlockLayer::LIQUID)
    {
        auto liquidity = block->getLiquidity();

        // If no liquid above (aka always render full block if there is liquid above it)
        if (!(liquidity & 0xF))
        {
            // If there is liquid in this block and to the left
            if (mod > 1 || (liquidity & 0xF00))
            {
                auto below = (liquidity >> 4) & 0xF;

                if (mod - 1 == below)
                {
                    mod = (liquidity >> 12) & 0xF;
                }
            }

            // Liquid animation is handled by the world renderer
            auto height  = (float)mod / 5.0F * BLOCK_SIZE;
            auto offsetY = BLOCK_SIZE - height;
            textureRect.origin.y += offsetY;
            textureRect.size.height = height;
            position.y -= offsetY;
        }

        // 0x1000A8575: Encode liquid animation data
        auto liquid = block->getLiquid();
        sprite->setTag((tileX << 16) | (tileY << 8) | liquid);

        // 0x1000A85C5: Hard coded opacity
        sprite->setOpacity(liquid == WATER ? 192 : 222);
    }

    // Offset position
    // HACK: Don't align maws & culverts
    if ((_layer != BlockLayer::BASE || tileable) && !modTile)
    {
        auto spriteX      = origin.x + (textureRect.size.x - BLOCK_SIZE) * 0.5F;
        auto spriteY      = origin.y + (textureRect.size.y - BLOCK_SIZE) * 0.5F;
        auto spriteOffset = math_util::rotateVector({spriteX, spriteY}, rotation);
        offset            = math_util::rotateVector(offset, rotation);

        if (aligned)
        {
            position.x += fabsf(spriteOffset.x) + (flipX ? -offset.x : offset.x);
        }

        position.y += fabsf(spriteOffset.y) - offset.y;
    }

    // Update sprite properties
    sprite->setAnchorPoint(Point::ANCHOR_MIDDLE);
    sprite->setTextureRect(textureRect);
    sprite->setPosition(position);
    sprite->setRotation(rotation);
    sprite->setFlippedX(flipX);
    sprite->setVisible(true);

    if (sprite->getLocalZOrder() != z)
    {
        _batchNode->reorderChild(sprite, z);
    }

    if (push)
    {
        block->pushSprite(sprite);
    }

    return sprite;
}

MaskedSprite* WorldLayerRenderer::getNextSprite()
{
    MaskedSprite* sprite = nullptr;

    if (_recycledSprites.empty())
    {
        sprite = MaskedSprite::createWithTexture(_batchNode->getTexture(), _batchNode->getMaskTexture());
        _batchNode->addChild(sprite);  // FIXME: Can cause double reordering
        sTotalSpriteCount++;
    }
    else
    {
        sprite = _recycledSprites.back();
        sprite->setFlippedX(false);
        sprite->setFlippedY(false);
        sprite->setScale(1.0F);
        sprite->setColor(Color3B::WHITE);
        sprite->setOpacity(0xFF);
        sprite->setTag(0);
        sprite->setMaskRect(_zeroMask);
        sprite->setMaskOrientation(MaskOrientation::DOWN);
        _recycledSprites.popBack();
    }

    return sprite;
}

MaskedSprite* WorldLayerRenderer::getNextSprite(const Rect& maskRect, MaskOrientation maskOrientation)
{
    auto sprite = getNextSprite();
    sprite->setMaskRect(maskRect);
    sprite->setMaskOrientation(maskOrientation);
    return sprite;
}

void WorldLayerRenderer::recycleSprite(MaskedSprite* sprite)
{
    AXASSERT(sprite, "Sprite can't be nullptr");
    AXASSERT(sprite->getParent() == _batchNode, "Sprite must belong to this layer renderer");

    // 0x100032A5F: Stop node actions
    if (sprite->getTag() == ACTION_SPRITE_TAG)
    {
        sprite->stopAllActions();
    }

    sprite->setVisible(false);
    _recycledSprites.pushBack(sprite);
}

}  // namespace opendw

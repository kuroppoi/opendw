#include "BaseBlock.h"

#include "graphics/backend/MaskedSprite.h"
#include "graphics/WorldLayerRenderer.h"
#include "graphics/WorldRenderer.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"
#include "GameConfig.h"
#include "GameManager.h"
#include "Item.h"

#define GLASS                    599
#define BALLOON                  607
#define BALLOON_STRIPED          678
#define SPECIAL_FRONT_CONTINUITY 1

USING_NS_AX;

namespace opendw
{

BaseBlock::~BaseBlock()
{
    // If this triggers then there is a bug in our zone cleanup loop that needs to be addressed
    AXASSERT(_sprites.empty(), "Sprites were not recycled properly!");
    sBlocksAllocated--;
}

BaseBlock* BaseBlock::createWithZone(WorldZone* zone, int16_t x, int16_t y)
{
    CREATE_INIT(BaseBlock, initWithZone, zone, x, y);
}

bool BaseBlock::initWithZone(WorldZone* zone, int16_t x, int16_t y)
{
    _zone      = zone;
    _x         = x;
    _y         = y;
    _placing   = true;
    _rendering = false;
    sBlocksAllocated++;
    return true;
}

void BaseBlock::setData(const ValueVector& data, uint32_t index)
{
    auto base     = data[(size_t)index * 3].asUint();
    auto back     = data[(size_t)index * 3 + 1].asUint();
    auto front    = data[(size_t)index * 3 + 2].asUint();
    _base         = base & 0xF;
    _liquid       = (base >> 8) & 0xFF;
    _liquidMod    = (base >> 16) & 0x1F;
    _back         = back & 0xFFFF;
    _backMod      = (back >> 16) & 0x1F;
    _front        = front & 0xFFFF;
    _frontMod     = (front >> 16) & 0x1F;
    _frontNatural = front < 0x100000;

    // 0x10002E853: This check is not necessary because opendw has a much higher item limit
    /*if (_front >= 2000)
    {
        AXLOGW("SUPER BAD WARNING: invalid item code {} detected at block front {} {}", _front, _x, _y);
    }*/

    auto config = GameManager::getInstance()->getConfig();
    _baseItem   = config->getItemForCode(_base);
    _liquidItem = config->getItemForCode(_liquid);
    updateBack();
    updateFront();
    // TODO: updateIllumination(false);
}

void BaseBlock::postPlace()
{
    updateEnvironment();
    _placing = false;
}

void BaseBlock::updateEnvironment(bool light, bool liquid, bool wholeness, bool continuity)
{
    if (liquid)
    {
        updateLiquid(true);
    }

    if (wholeness || continuity)
    {
        BaseBlock* neighbors[8];  // Stack allocation; no need to delete
        getNeighbors(neighbors);

        // 0x10002F96C: Update wholeness
        if (wholeness)
        {
            _wholeness = 0;

            for (uint8_t i = 0; i < 8; i++)
            {
                auto block = neighbors[i];
                _wholeness |= (!block || block->getFrontItem()->isWhole()) << i;
            }
        }

        // 0x10002FB7D: Update continuity
        if (continuity)
        {
            _baseContinuity     = 0;
            _backContinuity     = 0;
            _backModContinuity  = 0;
            _frontContinuity    = 0;
            _frontModContinuity = 0;

            for (uint8_t i = 0; i < 8; i++)
            {
                auto block = neighbors[i];

                // Corner blocks do not affect base continuity
                if (i < 4)
                {
                    _baseContinuity |= (!block || block->getBaseItem()->isContinuousFor(_baseItem)) << i;
                }

                _backContinuity |= (!block || block->getBackItem()->isContinuousFor(_backItem)) << i;
                _backModContinuity |= (block && block->getBackMod() > 0) << i;
                _frontContinuity |= (!block || block->getFrontItem()->isContinuousFor(_frontItem)) << i;
                _frontModContinuity |= (block && block->getFrontMod() > 0) << i;
            }

            // 0x10003046A: Handle special front continuity
#if SPECIAL_FRONT_CONTINUITY
            if (_front == GLASS || _front == BALLOON || _front == BALLOON_STRIPED)
            {
                // Unset top continuity if front is continuous with its right neighbor but not with its top right
                // neighbor OR if it is continuous with its left neighbor but not with its top left neighbor
                if ((_frontContinuity & (CONTINUITY_RIGHT | CONTINUITY_TOP_RIGHT)) == CONTINUITY_RIGHT ||
                    (_frontContinuity & (CONTINUITY_LEFT | CONTINUITY_TOP_LEFT)) == CONTINUITY_LEFT)
                {
                    _frontContinuity &= ~CONTINUITY_TOP;
                }

                // Unset bottom continuity if front is continuous with its right neighbor but not with its bottom right
                // neighbor OR if it is continuous with its left neighbor but not with its bottom left neighbor
                if ((_frontContinuity & (CONTINUITY_RIGHT | CONTINUITY_BOTTOM_RIGHT)) == CONTINUITY_RIGHT ||
                    (_frontContinuity & (CONTINUITY_LEFT | CONTINUITY_BOTTOM_LEFT)) == CONTINUITY_LEFT)
                {
                    _frontContinuity &= ~CONTINUITY_BOTTOM;
                }

                // Reset continuity if front is continuous with only its top OR bottom neighbor and one or both of its
                // respective corners
                auto edgeContinuity = _frontContinuity & CONTINUITY_EDGES;

                if ((edgeContinuity == CONTINUITY_TOP &&
                     _frontContinuity & (CONTINUITY_TOP_RIGHT | CONTINUITY_TOP_LEFT)) ||
                    (edgeContinuity == CONTINUITY_BOTTOM &&
                     _frontContinuity & (CONTINUITY_BOTTOM_RIGHT | CONTINUITY_BOTTOM_LEFT)))
                {
                    _frontContinuity = 0;
                }
            }
#endif  // SPECIAL_FRONT_CONTINUITY
        }
    }

    // 0x100030557: Queue block for rendering if necessary
    // TODO: it specifically checks for `placing != 1` (placing is set to 2 on chunk recycle)
    if (!_rendering && !_placing)
    {
        _zone->getWorldRenderer()->queueBlockForRendering(this);
    }
}

void BaseBlock::updateNeighbors()
{
    BaseBlock* neighbors[8];  // Stack allocation; no need to delete
    getNeighbors(neighbors);

    for (auto block : neighbors)
    {
        if (block)
        {
            block->updateEnvironment();
        }
    }
}

void BaseBlock::setLayer(BlockLayer layer, uint16_t item, uint8_t mod)
{
    switch (layer)
    {
    case BlockLayer::BASE:
        setBase(item);
        break;
    case BlockLayer::BACK:
        if (_back != item || _backMod != mod)
        {
            _back    = item;
            _backMod = mod;
            updateBack();
        }

        break;
    case BlockLayer::FRONT:
        if (_front != item || _frontMod != mod)
        {
            _front    = item;
            _frontMod = mod;
            updateFront();
        }

        break;
    case BlockLayer::LIQUID:
        mod = MIN(5, mod);

        if (_liquid != item || _liquidMod != mod)
        {
            _liquid     = item;
            _liquidMod  = mod;
            _liquidItem = GameManager::getInstance()->getConfig()->getItemForCode(_liquid);
            // TODO: updateIllumination(true);

            if (!_placing)
            {
                updateLiquid(true);
            }
        }

        break;
    }
}

void BaseBlock::setItemForLayer(BlockLayer layer, uint16_t item)
{
    switch (layer)
    {
    case BlockLayer::BASE:
        setBase((uint8_t)item);
        break;
    case BlockLayer::BACK:
        setBack(item);
        break;
    case BlockLayer::FRONT:
        setFront(item);
        break;
    case BlockLayer::LIQUID:
        setLiquid((uint8_t)item);
        break;
    }
}

Item* BaseBlock::getItemForLayer(BlockLayer layer) const
{
    switch (layer)
    {
    case BlockLayer::BASE:
        return _baseItem;
    case BlockLayer::BACK:
        return _backItem;
    case BlockLayer::FRONT:
        return _frontItem;
    case BlockLayer::LIQUID:
        return _liquidItem;
    default:
        return nullptr;
    }
}

void BaseBlock::setModForLayer(BlockLayer layer, uint8_t mod)
{
    switch (layer)
    {
    case BlockLayer::BACK:
        setBackMod(mod);
        break;
    case BlockLayer::FRONT:
        setFrontMod(mod);
        break;
    case BlockLayer::LIQUID:
        setLiquidMod(mod);
        break;
    }
}

uint8_t BaseBlock::getModForLayer(BlockLayer layer) const
{
    switch (layer)
    {
    case BlockLayer::BACK:
        return _backMod;
    case BlockLayer::FRONT:
        return _frontMod;
    case BlockLayer::LIQUID:
        return _liquidMod;
    default:
        return 0;
    }
}

uint8_t BaseBlock::getContinuityForLayer(BlockLayer layer) const
{
    switch (layer)
    {
    case BlockLayer::BASE:
        return _baseContinuity;
    case BlockLayer::BACK:
        return _backContinuity;
    case BlockLayer::FRONT:
        return _frontContinuity;
    default:
        return 0;
    }
}

void BaseBlock::setBase(uint8_t base)
{
    if (_base != base)
    {
        _base     = base;
        _baseItem = GameManager::getInstance()->getConfig()->getItemForCode(_base);

        if (!_placing)
        {
            updateEnvironment();
            updateNeighbors();
        }
    }
}

void BaseBlock::setBack(uint16_t back)
{
    if (_back != back)
    {
        _back = back;
        updateBack();
    }
}

void BaseBlock::setBackMod(uint8_t backMod)
{
    if (_backMod != backMod)
    {
        _backMod = backMod;
        updateBack();
    }
}

void BaseBlock::setFront(uint16_t front)
{
    if (_front != front)
    {
        _front = front;
        updateFront();
    }
}

void BaseBlock::setFrontMod(uint8_t frontMod)
{
    if (_frontMod != frontMod)
    {
        _frontMod = frontMod;
        updateFront();
    }
}

void BaseBlock::setLiquid(uint8_t liquid)
{
    if (_liquid != liquid)
    {
        _liquidItem = GameManager::getInstance()->getConfig()->getItemForCode(liquid);
        // TODO: updateIllumination(true);

        if (!_placing)
        {
            updateLiquid(true);
        }
    }
}

void BaseBlock::setLiquidMod(uint8_t liquidMod)
{
    liquidMod = MIN(5, liquidMod);

    if (_liquidMod != liquidMod)
    {
        _liquidMod = liquidMod;

        if (!_placing)
        {
            updateLiquid(true);
        }
    }
}

void BaseBlock::updateLiquid(bool updateNeighbors)
{
    _liquidity = 0;

    if (_liquid > 0)
    {
        // 0x1000305E0: Check volume of surrounding liquid blocks
        auto above = getAbove();
        auto below = getBelow();
        auto left  = getLeft();
        auto right = getRight();
        BaseBlock* blocks[4]{above, below, left, right};

        for (auto i = 0; i < 4; i++)
        {
            auto block = blocks[i];

            if (block && block->getLiquid() > 0)
            {
                _liquidity |= (block->getLiquidMod() & 0xF) << (i * 4);
            }
        }

        if (updateNeighbors)
        {
            if (left)
            {
                left->updateLiquid();
            }

            if (right)
            {
                right->updateLiquid();
            }
        }
    }
    else if (updateNeighbors)
    {
        auto left  = getLeft();
        auto right = getRight();

        if (left)
        {
            left->updateLiquid();
        }

        if (right)
        {
            right->updateLiquid();
        }
    }

    // It's faster to just render it directly rather than redrawing the entire block.
    // Liquid doesn't really interact with/affect any other layers anyway.
    _zone->getWorldRenderer()->updateLiquidInBlock(this);
}

void BaseBlock::updateFront()
{
    auto config = GameManager::getInstance()->getConfig();
    auto item   = config->getItemForCode(_front);
    AX_ASSERT(item);

    // 0x10002EFA8: Update change item
    if (item->getUseChangeItem() && _frontMod > 0)
    {
        item = item->getUseChangeItem();
    }
    else if (item->getModType() == ModType::CHANGE)
    {
        auto parent = item->getParentItem();

        if (parent)
        {
            item = parent;
        }

        if (_frontMod > 0)
        {
            auto& changeItems = item->getChangeItems();

            if (_frontMod <= changeItems.size())
            {
                item = changeItems[_frontMod - 1];  // Safe
                AX_ASSERT(item);
            }
        }
    }

    _front     = item->getCode();
    _frontItem = item;

    if (!_placing)
    {
        updateEnvironment();
        updateNeighbors();
        // TODO: updatePhysical();
    }
}

void BaseBlock::updateBack()
{
    auto config = GameManager::getInstance()->getConfig();
    auto item   = config->getItemForCode(_back);
    AX_ASSERT(item);

    // 0x10002F2DC: Update change item
    if (item->getUseChangeItem() && _backMod > 0)
    {
        item = item->getUseChangeItem();
    }
    else if (item->getModType() == ModType::CHANGE)
    {
        auto parent = item->getParentItem();

        if (parent)
        {
            item = parent;
        }

        if (_backMod > 0)
        {
            auto& changeItems = item->getChangeItems();

            if (_backMod <= changeItems.size())
            {
                item = changeItems[_backMod - 1];  // Safe
                AX_ASSERT(item);
            }
        }
    }

    _back     = item->getCode();
    _backItem = item;

    if (!_placing)
    {
        updateEnvironment();
        updateNeighbors();
    }
}

void BaseBlock::clearFromWorld()
{
    // TODO: clearPhysical();
    recycleSprites();
}

void BaseBlock::pushSprite(MaskedSprite* sprite)
{
    _sprites.pushBack(sprite);
}

void BaseBlock::recycleSprites(BlockLayer layer)
{
    // TODO: accessory sprites
    for (auto it = _sprites.begin(); it != _sprites.end();)
    {
        auto sprite = *it;
        auto parent = sprite->getParent();

        // Can happen when biome renderers remove all of their sprites
        if (!parent)
        {
            it = _sprites.erase(it);
            continue;
        }

        auto renderer = static_cast<WorldLayerRenderer*>(parent->getParent());

        if (layer == BlockLayer::NONE || renderer->getLayer() == layer)
        {
            renderer->recycleSprite(sprite);
            it = _sprites.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void BaseBlock::recycleSpriteWithTag(int tag)
{
    // Removing while iterating should be slightly faster than using getSpriteWithTag
    for (auto it = _sprites.begin(); it != _sprites.end(); it++)
    {
        auto sprite = *it;

        if (sprite->getTag() == tag)
        {
            auto renderer = static_cast<WorldLayerRenderer*>(sprite->getParent()->getParent());
            renderer->recycleSprite(sprite);
            _sprites.erase(it);
            break;
        }
    }
}

MaskedSprite* BaseBlock::getSpriteWithTag(int tag) const
{
    for (auto& sprite : _sprites)
    {
        if (sprite->getTag() == tag)
        {
            return sprite;
        }
    }

    return nullptr;
}

MaskedSprite* BaseBlock::getTopSpriteForLayer(BlockLayer layer) const
{
    MaskedSprite* topSprite = nullptr;

    for (auto& sprite : _sprites)
    {
        auto renderer = static_cast<WorldLayerRenderer*>(sprite->getParent()->getParent());

        if (renderer->getLayer() == layer && (!topSprite || sprite->getLocalZOrder() >= topSprite->getLocalZOrder()))
        {
            topSprite = sprite;
        }
    }

    return topSprite;
}

bool BaseBlock::isOpaque() const
{
    return isBackOpaque() && isFrontOpaque();
}

bool BaseBlock::isOpaque(BlockLayer layer) const
{
    switch (layer)
    {
    case BlockLayer::BACK:
        return isBackOpaque();
    case BlockLayer::FRONT:
        return isFrontOpaque();
    default:
        return true;
    }
}

bool BaseBlock::isBackOpaque() const
{
    return _backItem->isOpaque() && (_backItem->getModType() != ModType::DECAY || _backMod < 2);
}

bool BaseBlock::isFrontOpaque() const
{
    return _frontItem->isOpaque() && (_frontItem->getModType() != ModType::DECAY || _frontMod < 2);
}

BaseBlock* BaseBlock::getAbove() const
{
    return _zone->getBlockAt(_x, _y - 1);
}

BaseBlock* BaseBlock::getBelow() const
{
    return _zone->getBlockAt(_x, _y + 1);
}

BaseBlock* BaseBlock::getLeft() const
{
    return _zone->getBlockAt(_x - 1, _y);
}

BaseBlock* BaseBlock::getRight() const
{
    return _zone->getBlockAt(_x + 1, _y);
}

void BaseBlock::getNeighbors(BaseBlock* neighbors[8]) const
{
    // We're just going to assume that there's enough room for 8
    neighbors[0] = _zone->getBlockAt(_x, _y - 1);      // Top
    neighbors[1] = _zone->getBlockAt(_x + 1, _y);      // Right
    neighbors[2] = _zone->getBlockAt(_x, _y + 1);      // Bottom
    neighbors[3] = _zone->getBlockAt(_x - 1, _y);      // Left
    neighbors[4] = _zone->getBlockAt(_x + 1, _y - 1);  // Top right
    neighbors[5] = _zone->getBlockAt(_x + 1, _y + 1);  // Bottom right
    neighbors[6] = _zone->getBlockAt(_x - 1, _y + 1);  // Bottom left
    neighbors[7] = _zone->getBlockAt(_x - 1, _y - 1);  // Top left
}

}  // namespace opendw

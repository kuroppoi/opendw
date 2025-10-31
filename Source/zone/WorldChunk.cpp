#include "WorldChunk.h"

#include "zone/BaseBlock.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

WorldChunk::~WorldChunk()
{
    for (uint32_t i = 0; i < _count; i++)
    {
        AX_SAFE_RELEASE(_blocks[i]);
    }

    AX_SAFE_DELETE_ARRAY(_blocks);
    sChunksAllocated--;
}

WorldChunk* WorldChunk::createWithZone(WorldZone* zone, uint16_t x, uint16_t y, uint32_t count)
{
    CREATE_INIT(WorldChunk, initWithZone, zone, x, y, count);
}

bool WorldChunk::initWithZone(WorldZone* zone, uint16_t x, uint16_t y, uint32_t count)
{
    AXASSERT(!_blocks, "Reinitialization is not allowed");
    _zone   = zone;
    _count  = count;
    _blocks = new BaseBlock*[count];

    for (uint32_t i = 0; i < count; i++)
    {
        auto block = BaseBlock::createWithZone(zone, 0, 0);  // Block positions will be set later
        block->retain();
        _blocks[i] = block;
    }

    setPosition(x, y);
    sChunksAllocated++;
    return true;
}

void WorldChunk::setPosition(uint16_t x, uint16_t y)
{
    _x      = x;
    _y      = y;
    _index  = y * _zone->getChunkCountX() + x;
    _blockX = x * _zone->getChunkWidth();
    _blockY = y * _zone->getChunkHeight();

    for (uint32_t i = 0; i < _count; i++)
    {
        auto block = _blocks[i];
        block->setX(_blockX + i % _zone->getChunkWidth());
        block->setY(_blockY + i / _zone->getChunkWidth());
    }

    _beganAt = utils::gettime();
}

void WorldChunk::recycle()
{
    for (uint32_t i = 0; i < _count; i++)
    {
        auto block = _blocks[i];
        block->setPlacing(true);  // TODO: originally sets it to 2
        block->clearFromWorld();
    }
}

BaseBlock* WorldChunk::getBlockAt(uint16_t x, uint16_t y)
{
    auto index = y * _zone->getChunkWidth() + x;

    if (index < _count)
    {
        return _blocks[index];
    }

    return nullptr;
}

}  // namespace opendw

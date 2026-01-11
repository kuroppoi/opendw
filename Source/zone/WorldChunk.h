#ifndef __WORLD_CHUNK_H__
#define __WORLD_CHUNK_H__

#include "axmol.h"

namespace opendw
{

class BaseBlock;
class WorldZone;

/*
 * CLASS: WorldChunk : NSObject @ 0x100318958
 */
class WorldChunk : public ax::Object
{
public:
    /* FUNC: WorldChunk::dealloc @ 0x1000CAAC6 */
    ~WorldChunk() override;

    static WorldChunk* createWithZone(WorldZone* zone, int16_t x, int16_t y, uint32_t count);

    /* FUNC: WorldChunk::chunksAllocated @ 0x1000CAF6F */
    static size_t getChunksAllocated() { return sChunksAllocated; }

    /* FUNC: WorldChunk::initWithZone:x:y:count: @ 0x1000CA710 */
    bool initWithZone(WorldZone* zone, int16_t x, int16_t y, uint32_t count);

    /* FUNC: WorldChunk::setX:y: @ 0x1000CA84F */
    void setPosition(int16_t x, int16_t y);

    /* FUNC: WorldChunk::recycle @ 0x1000CA9D8 */
    void recycle();

    BaseBlock* getBlockAt(int16_t x, int16_t y);

    /* FUNC: WorldChunk::zone @ 0x1000CAB61 */
    WorldZone* getZone() const { return _zone; }

    /* FUNC: WorldChunk::count @ 0x1000CABC7 */
    uint32_t getCount() const { return _count; }

    /* FUNC: WorldChunk::x @ 0x1000CAB72 */
    int16_t getX() const { return _x; }

    /* FUNC: WorldChunk::y @ 0x1000CAB83 */
    int16_t getY() const { return _y; }

    /* FUNC: WorldChunk::blockX @ 0x1000CAB94 */
    int16_t getBlockX() const { return _blockX; }

    /* FUNC: WorldChunk::blockY @ 0x1000CABA5 */
    int16_t getBlockY() const { return _blockY; }

    /* FUNC: WorldChunk::idx @ 0x1000CABB6 */
    int32_t getIndex() const { return _index; }

    /* FUNC: WorldChunk::beganAt @ 0x1000CABE9 */
    double getBeganAt() const { return _beganAt; }

private:
    inline static size_t sChunksAllocated = 0;  // 0x100334818

    WorldZone* _zone;     // WorldChunk::zone @ 0x100312B48
    uint32_t _count;      // WorldChunk::count @ 0x100312B50
    BaseBlock** _blocks;  // WorldChunk::blocks @ 0x100312B58
    int16_t _x;           // WorldChunk::x @ 0x100312B60
    int16_t _y;           // WorldChunk::y @ 0x100312B68
    int16_t _blockX;      // WorldChunk::blockX @ 0x100312B78
    int16_t _blockY;      // WorldChunk::blockY @ 0x100312B80
    int32_t _index;       // WorldChunk::idx @ 0x100312B70
    double _beganAt;      // WorldChunk::beganAt @ 0x100312B88
};

}  // namespace opendw

#endif  // __WORLD_CHUNK_H__

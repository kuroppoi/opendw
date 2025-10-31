#include "GameCommandBlocks.h"

#include "graphics/WorldRenderer.h"
#include "zone/BaseBlock.h"
#include "zone/WorldZone.h"
#include "GameManager.h"

#define MAX_FRAME_TIME 0.00075

USING_NS_AX;

namespace opendw
{

void GameCommandBlocks::run()
{
    auto zone     = GameManager::getInstance()->getZone();
    auto renderer = zone->getWorldRenderer();
    auto start    = utils::gettime();

    for (; _currentChunk < _chunkCount; _currentChunk++)
    {
        if (utils::gettime() >= start + MAX_FRAME_TIME)
        {
            break;
        }

        auto& chunk         = _data[_currentChunk].asValueVector();
        uint16_t x          = chunk[0].asUint();
        uint16_t y          = chunk[1].asUint();
        uint16_t width      = chunk[2].asUint();
        uint16_t height     = chunk[3].asUint();
        auto& blocks        = chunk[4].asValueVector();
        uint32_t blockCount = width * height;
        AX_ASSERT(blocks.size() / 3 == blockCount);

        for (uint32_t i = 0; i < blockCount; i++)
        {
            uint16_t blockX = x + (i % width);
            uint16_t blockY = y + i / width;
            auto block      = zone->getBlockAt(blockX, blockY, true);
            AX_ASSERT(block);
            block->setData(blocks, i);

            // 0x1000E1B32: Render immediately if block is visible
            if (renderer->isBlockInViewport(block))
            {
                renderer->queueBlockForRendering(block);
            }
        }

        zone->removePendingChunk(x, y);
    }
}

void GameCommandBlocks::postUnpack()
{
    _chunkCount   = _data.size();
    _currentChunk = 0;
}

bool GameCommandBlocks::isDone() const
{
    return _currentChunk >= _chunkCount;
}

}  // namespace opendw

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

        auto& chunk     = _data[_currentChunk].asValueVector();
        auto x          = chunk[0].asInt();
        auto y          = chunk[1].asInt();
        auto width      = chunk[2].asInt();
        auto height     = chunk[3].asInt();
        auto& blocks    = chunk[4].asValueVector();
        auto blockCount = width * height;
        AX_ASSERT(blocks.size() / 3 == blockCount);

        for (auto i = 0; i < blockCount; i++)
        {
            auto blockX = x + (i % width);
            auto blockY = y + i / width;
            auto block  = zone->getBlockAt(blockX, blockY, true);
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

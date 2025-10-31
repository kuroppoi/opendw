#include "GameCommandBlockChange.h"

#include "zone/BaseBlock.h"
#include "zone/WorldZone.h"
#include "GameManager.h"

#define MAX_FRAME_TIME 0.00075

USING_NS_AX;

namespace opendw
{

void GameCommandBlockChange::run()
{
    auto zone  = GameManager::getInstance()->getZone();
    auto start = utils::gettime();

    for (; _currentBlock < _blockCount; _currentBlock++)
    {
        // 0x1000E169E: Process remaining blocks next frame if we're taking too long
        if (utils::gettime() >= start + MAX_FRAME_TIME)
        {
            break;
        }

        // TODO: test against player entity ID
        auto& data     = _data[_currentBlock].asValueVector();
        auto x         = data[0].asUint();
        auto y         = data[1].asUint();
        auto layer     = static_cast<BlockLayer>(1 + data[2].asByte());
        auto& entityId = data[3];  // Nullable
        auto& item     = data[4];  // Nullable
        auto& mod      = data[5];  // Nullable
        auto block     = zone->getBlockAt(x, y);

        if (!block)
        {
            AXLOGD("[GameCommandBlockChange] Block at {} {} is nullptr!", x, y);
            continue;
        }

        // 0x1000E17B6: Update the block based on which data is present
        if (!item.isNull())
        {
            if (!mod.isNull())
            {
                // Update both item and mod
                block->setLayer(layer, item.asUint(), mod.asUint());
            }
            else
            {
                // Only update item
                block->setItemForLayer(layer, item.asUint());
            }
        }
        else if (!mod.isNull())
        {
            // Only update mod
            block->setModForLayer(layer, mod.asByte());
        }
    }
}

void GameCommandBlockChange::postUnpack()
{
    _blockCount   = _data.size();
    _currentBlock = 0;
}

bool GameCommandBlockChange::isDone() const
{
    return _currentBlock >= _blockCount;
}

}  // namespace opendw

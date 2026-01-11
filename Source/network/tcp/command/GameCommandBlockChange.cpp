#include "GameCommandBlockChange.h"

#include "zone/BaseBlock.h"
#include "zone/WorldZone.h"
#include "GameManager.h"
#include "Player.h"

#define MAX_FRAME_TIME 0.00075

USING_NS_AX;

namespace opendw
{

void GameCommandBlockChange::run()
{
    auto game   = GameManager::getInstance();
    auto zone   = game->getZone();
    auto player = game->getPlayer();
    auto start  = utils::gettime();

    for (; _currentBlock < _blockCount; _currentBlock++)
    {
        // 0x1000E169E: Process remaining blocks next frame if we're taking too long
        if (utils::gettime() >= start + MAX_FRAME_TIME)
        {
            break;
        }

        auto& data     = _data[_currentBlock].asValueVector();
        auto& entityId = data[3];  // Nullable

        // 0x1000E171B: Skip if block change was caused by us
        if (!entityId.isNull() && entityId.asInt() == player->getEntityId())
        {
            continue;
        }

        auto x     = data[0].asInt();
        auto y     = data[1].asInt();
        auto layer = static_cast<BlockLayer>(1 + data[2].asByte());
        auto& item = data[4];  // Nullable
        auto& mod  = data[5];  // Nullable
        auto block = zone->getBlockAt(x, y);

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

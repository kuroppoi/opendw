#include "MetaBlock.h"

#include "base/Item.h"
#include "base/Player.h"
#include "util/MapUtil.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

MetaBlock* MetaBlock::createWithData(int16_t x, int16_t y, Item* item, const ValueMap& metadata)
{
    CREATE_INIT(MetaBlock, initWithData, x, y, item, metadata);
}

bool MetaBlock::initWithData(int16_t x, int16_t y, Item* item, const ValueMap& metadata)
{
    _x    = x;
    _y    = y;
    _item = item;
    setMetadata(metadata);
    return true;
}

bool MetaBlock::isOwnedByPlayer() const
{
    return _playerId == Player::getMain()->getPlayerId();
}

bool MetaBlock::isOwnedByPlayerOrFollower() const
{
    // TODO: client doesn't track followers yet
    return isOwnedByPlayer();
}

void MetaBlock::setMetadata(const ValueMap& metadata)
{
    _metadata = metadata;  // Create copy
    _playerId = map_util::getString(metadata, "p");
}

}  // namespace opendw

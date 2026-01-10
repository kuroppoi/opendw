#include "GameCommandBlockMeta.h"

#include "util/MapUtil.h"
#include "zone/WorldZone.h"
#include "GameConfig.h"
#include "Item.h"

USING_NS_AX;

namespace opendw
{

void GameCommandBlockMeta::run()
{
    auto zone   = WorldZone::getMain();
    auto config = GameConfig::getMain();

    for (size_t i = 0; i < _data.size(); i++)
    {
        auto& data     = _data[i].asValueVector();
        auto x         = data[0].asInt();
        auto y         = data[1].asInt();
        auto& metadata = data[2].isNull() ? ValueMapNull : data[2].asValueMap();
        auto code      = map_util::getInt32(metadata, "i", -1);
        auto item      = code == -1 ? nullptr : config->getItemForCode(code);
        zone->setMetaBlock(x, y, item, metadata);
    }
}

}  // namespace opendw

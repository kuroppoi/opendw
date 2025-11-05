#include "GameCommandZoneSearch.h"

#include "gui/GameGui.h"
#include "gui/TeleportIcon.h"  // ZoneSearchInfo
#include "util/Validation.h"

#define SEARCH_INFO_DESCRIPTOR "SSNNANNS[Sx]SN[Sx]"

USING_NS_AX;

namespace opendw
{

void GameCommandZoneSearch::run()
{
    // NOTE: The original implementation passes the data directly to GameGui,
    // but we're gonna deserialize it into a vector of structs first.

    auto type  = _data[0].asString();
    auto& data = _data[3].asValueVector();
    std::vector<ZoneSearchInfo> zoneInfo;
    zoneInfo.reserve(data.size());

    // Convert data to struct
    for (auto& element : data)
    {
        AX_ASSERT(element.getType() == Value::Type::VECTOR);
        auto& array = element.asValueVector();

        if (!validation::validateArray(array, SEARCH_INFO_DESCRIPTOR))
        {
            continue;
        }

        ZoneSearchInfo info;
        info.documentId    = array[0].asString();
        info.name          = array[1].asString();
        info.playerCount   = array[2].asInt();
        info.followeeCount = array[3].asInt();
        info.explored      = array[6].asInt();
        info.biome         = array[7].asString();
        info.status        = array[8].asString();
        info.accessibility = array[9].asString();
        info.protection    = array[10].asInt();
        info.scenario      = array[11].asString();
        zoneInfo.push_back(info);
    }

    GameGui::getMain()->showTeleportZones(type, zoneInfo);
}

}  // namespace opendw

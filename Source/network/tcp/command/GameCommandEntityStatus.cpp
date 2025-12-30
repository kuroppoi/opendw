#include "GameCommandEntityStatus.h"

#include "util/MapUtil.h"
#include "zone/WorldZone.h"
#include "Player.h"

USING_NS_AX;

namespace opendw
{

enum Status : uint8_t
{
    EXITING,
    ENTERING,
    DEAD,
    REVIVED
};

void GameCommandEntityStatus::run()
{
    auto zone   = WorldZone::getMain();
    auto player = Player::getMain();

    if (!zone)
    {
        AXLOGW("Warning - entity status arriving early");
        return;
    }

    for (auto& element : _data)
    {
        auto& data    = element.asValueVector();
        auto entityId = data[0].asInt();

        // 0x1000E4A8E: Update player avatar
        if (entityId == player->getEntityId())
        {
            if (zone->getState() == WorldZone::State::ACTIVE)
            {
                // TODO: implement
            }

            return;
        }

        auto code     = data[1].isNull() ? 0 : data[1].asInt();
        auto name     = data[2].isNull() ? STD_STRING_EMPTY : data[2].asString();
        auto status   = static_cast<Status>(data[3].asInt());
        auto& details = data[4].isNull() ? ValueMapNull : data[4].asValueMap();

        // 0x1000E4BBF: Handle based on status
        switch (status)
        {
        case EXITING:
        {
            auto violent = map_util::getString(details, "!") == "v";
            zone->removeEntity(entityId, violent);
            break;
        }
        case ENTERING:
            zone->registerEntity(entityId, code, name, details);
            break;
        case DEAD:
            // TODO: implement
            break;
        case REVIVED:
            // TODO: implement
            break;
        }
    }
}

}  // namespace opendw

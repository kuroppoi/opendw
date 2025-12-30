#include "GameCommandEntityChange.h"

#include "entity/Entity.h"
#include "zone/WorldZone.h"
#include "Player.h"

namespace opendw
{

void GameCommandEntityChange::run()
{
    auto zone = WorldZone::getMain();

    if (!zone)
    {
        AXLOGW("Warning - entity change arriving early");
        return;
    }

    for (auto& element : _data)
    {
        auto& data    = element.asValueVector();
        auto entityId = data[0].asInt();
        auto& details = data[1].asValueMap();
        auto entity   = zone->getEntityById(entityId);

        if (entity)
        {
            entity->change(details);
        }
        else
        {
            // TODO: update player directing state (butler bots?)
        }
    }
}

}  // namespace opendw

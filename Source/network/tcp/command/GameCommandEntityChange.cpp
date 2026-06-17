#include "GameCommandEntityChange.h"

#include "base/Player.h"
#include "entity/Entity.h"
#include "entity/EntityAnimatedAvatar.h"
#include "zone/WorldZone.h"

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

        if (!entity)
        {
            auto player = Player::getMain();

            if (player->getEntityId() == entityId)
            {
                entity = player->getAvatar();
                // TODO: update directing state
            }
        }

        if (entity)
        {
            entity->change(details);
        }
    }
}

}  // namespace opendw

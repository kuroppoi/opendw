#include "GameCommandEntityPosition.h"

#include "entity/Entity.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"

namespace opendw
{

void GameCommandEntityPosition::run()
{
    // TODO: finish
    auto zone = WorldZone::getMain();

    if (!zone)
    {
        AXLOGW("Warning - entity position arriving early");
        return;
    }

    for (auto& element : _data)
    {
        auto& data    = element.asValueVector();
        auto entityId = data[0].asInt();
        auto entity   = zone->getEntityById(entityId);

        if (!entity)
        {
            // TODO: request entity status
            continue;
        }

        auto x = data[1].asFloat() / 100.0F * BLOCK_SIZE;
        auto y = -data[2].asFloat() / 100.0F * BLOCK_SIZE - BLOCK_SIZE * 0.5F;

        if (!entity->isAvatar())
        {
            x += BLOCK_SIZE * 0.5F;
        }

        auto direction = (int8_t)data[5].asInt();
        auto animation = data[8].asInt();
        entity->setRealPosition({x, y});
        entity->runAnimation(animation);

        if (!entity->isBlock())
        {
            // TODO: check if human
            entity->setFlippedX(direction > 0 ? false : true);
        }
    }
}

}  // namespace opendw

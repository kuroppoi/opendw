#include "GameCommandZoneExplored.h"

#include "zone/WorldZone.h"

namespace opendw
{

void GameCommandZoneExplored::run()
{
    auto zone = WorldZone::getMain();

    if (zone->getState() == WorldZone::State::ACTIVE)
    {
        auto& chunks = _data[0].asValueVector();

        for (auto& chunk : chunks)
        {
            zone->setChunkExplored(chunk.asInt());
        }
    }
}

}  // namespace opendw

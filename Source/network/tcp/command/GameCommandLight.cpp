#include "GameCommandLight.h"

#include "zone/WorldZone.h"

USING_NS_AX;

namespace opendw
{

void GameCommandLight::run()
{
    auto zone = WorldZone::getMain();

    if (!zone)
    {
        return;
    }

    for (auto& element : _data)
    {
        auto& data  = element.asValueVector();
        auto x      = data[0].asInt();
        auto& light = data[3].asValueVector();

        for (auto i = 0; i < light.size(); i++)
        {
            auto depth = light[i].asInt();
            zone->updateSunlight(x + i, depth);
        }
    }
}

}  // namespace opendw

#include "GameCommandZoneStatus.h"

#include "zone/WorldZone.h"
#include "GameManager.h"

namespace opendw
{

void GameCommandZoneStatus::run()
{
    auto zone = GameManager::getInstance()->getZone();

    if (!zone)
    {
        AXLOGW("[GameCommandZoneStatus] Zone is nullptr!");
        return;
    }

    auto& status = _data[0].asValueMap();
    zone->updateStatus(status);
}

}  // namespace opendw

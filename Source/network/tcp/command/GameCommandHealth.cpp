#include "GameCommandHealth.h"

#include "base/Player.h"
#include "zone/WorldZone.h"

namespace opendw
{

void GameCommandHealth::run()
{
    auto health        = _data[0].asFloat();
    auto player        = Player::getMain();
    auto currentHealth = player->getHealth();

    if (health < currentHealth)
    {
        auto zone = WorldZone::getMain();

        if (zone && zone->getState() == WorldZone::State::ACTIVE)
        {
            // TODO: animate hurt
            player->playHurtSound(currentHealth - health >= 1.0F);
        }
    }

    player->setHealth(health);
}

}  // namespace opendw

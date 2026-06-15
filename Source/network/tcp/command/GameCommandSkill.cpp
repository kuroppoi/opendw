#include "GameCommandSkill.h"

#include "base/Player.h"
#include "event/EventNames.h"
#include "zone/WorldZone.h"
#include "GameManager.h"

namespace opendw
{

void GameCommandSkill::run()
{
    auto game   = GameManager::getInstance();
    auto player = game->getPlayer();
    auto zone   = game->getZone();

    for (auto& element : _data)
    {
        auto& data = element.asValueVector();
        auto skill = data[0].asString();
        auto level = data[1].asInt();
        player->setSkill(skill, level);
    }

    if (zone->getState() != WorldZone::State::ACTIVE)
    {
        game->getEventDispatcher()->dispatchCustomEvent(events::kPlayerSkillChanged);
    }
}

}  // namespace opendw

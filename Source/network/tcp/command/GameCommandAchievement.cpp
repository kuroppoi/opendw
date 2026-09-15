#include "GameCommandAchievement.h"

#include "base/Player.h"

namespace opendw
{

void GameCommandAchievement::run()
{
    for (auto& element : _data)
    {
        auto& data  = element.asValueVector();
        auto name   = data[0].asString();
        auto points = data[1].asInt();
        Player::getMain()->addAchievement(name, points);
    }
}

}  // namespace opendw

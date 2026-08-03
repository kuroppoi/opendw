#include "GameCommandStat.h"

#include "base/Player.h"

namespace opendw
{

void GameCommandStat::run()
{
    // TODO: finish, implement other stats

    auto player = Player::getMain();

    for (auto& element : _data)
    {
        auto& data  = element.asValueVector();
        auto stat   = data[0].asString();
        auto& value = data[1];
        
        if (stat == "freeze")
        {
            player->setFreeze(value.asFloat());
        }
        else if (stat == "breath")
        {
            player->setBreath(value.asFloat());
        }
    }
}

}  // namespace opendw

#ifndef __GAME_COMMAND_HEALTH_H__
#define __GAME_COMMAND_HEALTH_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandHealth : GameCommand @ 0x10031A550
 */
class GameCommandHealth : public GameCommand
{
public:
    /* FUNC: GameCommandHealth::run @ 0x1000FB5E3 */
    void run() override;

    /* FUNC: GameCommandHealth::validArrayDataDescriptor @ 0x1000FB5D6 */
    const char* getDataDescriptor() const override { return "N"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_HEALTH_H__

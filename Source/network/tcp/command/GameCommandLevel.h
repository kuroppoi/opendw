#ifndef __GAME_COMMAND_LEVEL_H__
#define __GAME_COMMAND_LEVEL_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandLevel : GameCommand @ 0x100317A30
 */
class GameCommandLevel : public GameCommand
{
public:
    /* FUNC: GameCommandLevel::run @ 0x100096160 */
    void run() override;

    const char* getDataDescriptor() const override { return "N"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_LEVEL_H__

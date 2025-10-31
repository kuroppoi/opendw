#ifndef __GAME_COMMAND_PLAYER_POSITION_H__
#define __GAME_COMMAND_PLAYER_POSITION_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandPlayerPosition : GameCommand @ 0x10031C5D0
 */
class GameCommandPlayerPosition : public GameCommand
{
public:
    /* FUNC: GameCommandPlayerPosition::run @ 0x100143AC6 */
    void run() override;

    const char* getDataDescriptor() const override { return "NN"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_PLAYER_POSITION_H__

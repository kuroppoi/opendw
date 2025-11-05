#ifndef __GAME_COMMAND_KICK_H__
#define __GAME_COMMAND_KICK_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandKick : GameCommand @ 0x10031C490
 */
class GameCommandKick : public GameCommand
{
public:
    /* FUNC: GameCommandKick::run @ 0x100143623 */
    void run() override;

    /* FUNC: GameCommandKick::highPriorty @ 0x100143618 */
    bool isHighPriorty() const override { return true; }

    const char* getDataDescriptor() const override { return "SN"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_KICK_H__

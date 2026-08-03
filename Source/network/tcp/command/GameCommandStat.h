#ifndef __GAME_COMMAND_STAT_H__
#define __GAME_COMMAND_STAT_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandStat : GameCommand @ 0x10031CA30
 */
class GameCommandStat : public GameCommand
{
public:
    /* FUNC: GameCommandStat::run @ 0x10014D140 */
    void run();

    /* FUNC: GameCommandStat::collection @ 0x10014D128 */
    bool isCollection() const override { return true; }

    /* FUNC: GameCommandStat::validArrayDataDescriptor @ 0x10014D133 */
    const char* getDataDescriptor() const override { return "S."; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_STAT_H__

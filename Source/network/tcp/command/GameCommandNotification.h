#ifndef __GAME_COMMAND_NOTIFICATION_H__
#define __GAME_COMMAND_NOTIFICATION_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandNotification : GameCommand @ 0x10031C4E0
 */
class GameCommandNotification : public GameCommand
{
public:
    /* FUNC: GameCommandNotification::run @ 0x10014373F */
    void run() override;

    const char* getDataDescriptor() const override { return ".N"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_NOTIFICATION_H__

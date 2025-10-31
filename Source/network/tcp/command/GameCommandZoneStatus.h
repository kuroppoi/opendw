#ifndef __GAME_COMMAND_ZONE_STATUS_H__
#define __GAME_COMMAND_ZONE_STATUS_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandZoneStatus @ 0x100319BF0
 */
class GameCommandZoneStatus : public GameCommand
{
public:
    /* FUNC: GameCommandZoneStatus::run @ 0x1000EACFC */
    void run() override;

    /* FUNC: GameCommandZoneStatus::validArrayDataDescriptor @ 0x1000EACD8 */
    const char* getDataDescriptor() const override { return "D"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_ZONE_STATUS_H__

#ifndef __GAME_COMMAND_ZONE_EXPLORED_H__
#define __GAME_COMMAND_ZONE_EXPLORED_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandZoneExplored : GameCommand @ 0x10031FFA0
 */
class GameCommandZoneExplored : public GameCommand
{
public:
    /* FUNC: GameCommandZoneExplored::run @ 0x1001A6A12 */
    void run() override;

    /* FUNC: GameCommandZoneExplored::collection @ 0x1001A69E3 */
    bool isCollection() const override { return true; }

    /* FUNC: GameCommandZoneExplored::validArrayDataDescriptor @ 0x1001A69EE */
    const char* getDataDescriptor() const override { return "N"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_ZONE_EXPLORED_H__

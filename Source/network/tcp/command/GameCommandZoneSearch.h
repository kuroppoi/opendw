#ifndef __GAME_COMMAND_ZONE_SEARCH_H__
#define __GAME_COMMAND_ZONE_SEARCH_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandZoneSearch : GameCommand @ 0x10031A780
 */
class GameCommandZoneSearch : public GameCommand
{
public:
    /* FUNC: GameCommandZoneSearch::run @ 0x100101A8D */
    void run() override;

    const char* getDataDescriptor() const override { return "SNNAN"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_ZONE_SEARCH_H__

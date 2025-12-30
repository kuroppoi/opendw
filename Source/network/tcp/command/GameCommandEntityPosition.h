#ifndef __GAME_COMMAND_ENTITY_POSITION_H__
#define __GAME_COMMAND_ENTITY_POSITION_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandEntityPosition : GameCommand @ 0x10031C350
 */
class GameCommandEntityPosition : public GameCommand
{
public:
    /* FUNC: GameCommandEntityPosition::run @ 0x10013E710 */
    void run() override;

    /* FUNC: GameCommandEntityPosition::collection @ 0x10013E6C1 */
    bool isCollection() const override { return true; }

    /* FUNC: GameCommandEntityPosition::validArrayDataDescriptor @ 0x10013E6D7 */
    const char* getDataDescriptor() const override { return "NNNNN[Nx][Nx][Nx][Nx]"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_ENTITY_POSITION_H__

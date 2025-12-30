#ifndef __GAME_COMMAND_ENTITY_STATUS_H__
#define __GAME_COMMAND_ENTITY_STATUS_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandEntityStatus : GameCommand @ 0x100319420
 */
class GameCommandEntityStatus : public GameCommand
{
public:
    /* GameCommandEntityStatus::run @ 0x1000E4773 */
    void run() override;

    /* FUNC: GameCommandEntityStatus::collection @ 0x1000E4750 */
    bool isCollection() const override { return true; }

    /* FUNC: GameCommandEntityStatus::validArrayDataDescriptor @ 0x1000E4766 */
    const char* getDataDescriptor() const override { return "N[Nx][Sx]N[Dx]"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_ENTITY_STATUS_H__

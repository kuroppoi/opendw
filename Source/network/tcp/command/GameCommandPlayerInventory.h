#ifndef __GAME_COMMAND_PLAYER_INVENTORY_H__
#define __GAME_COMMAND_PLAYER_INVENTORY_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandPlayerInventory : GameCommand @ 0x10031B400
 */
class GameCommandPlayerInventory : public GameCommand
{
public:
    /* FUNC: GameCommandPlayerInventory::run @ 0x10012D7D7 */
    void run() override;

    /* FUNC: GameCommandPlayerInventory::validArrayDataDescriptor @ 0x10012DCEB */
    const char* getDataDescriptor() const override { return "D"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_PLAYER_INVENTORY_H__

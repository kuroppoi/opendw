#ifndef __GAME_COMMAND_ENTITY_CHANGE_H__
#define __GAME_COMMAND_ENTITY_CHANGE_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandEntityChange : GameCommand @ 0x10031C300
 */
class GameCommandEntityChange : public GameCommand
{
public:
    /* FUNC: GameCommandEntityChange::run @ 0x10013E3E0 */
    void run() override;

    /* FUNC: GameCommandEntityChange::collection @ 0x10013E3C8 */
    bool isCollection() const override { return true; }

    /* FUNC: GameCommandEntityChange::validArrayDataDescriptor @ 0x10013E3D3 */
    const char* getDataDescriptor() const override { return "ND"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_ENTITY_CHANGE_H__

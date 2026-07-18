#ifndef __GAME_COMMAND_DIALOG_H__
#define __GAME_COMMAND_DIALOG_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandDialog : GameCommand @ 0x10031D020
 */
class GameCommandDialog : public GameCommand
{
public:
    /* FUNC: GameCommandDialog::run @ 0x10015159B */
    void run() override;

    /* FUNC: GameCommandDialog::compressed @ 0x100151583 */
    bool isCompressed() const override { return true; }

    /* FUNC: GameCommandDialog::validArrayDataDescriptor @ 0x10015158E */
    const char* getDataDescriptor() const override { return "ND"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_DIALOG_H__

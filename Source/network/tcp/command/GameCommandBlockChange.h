#ifndef __GAME_COMMAND_BLOCK_CHANGE_H__
#define __GAME_COMMAND_BLOCK_CHANGE_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandBlockChange : GameCommand @ 0x100318F20
 */
class GameCommandBlockChange : public GameCommand
{
public:
    /* FUNC: GameCommandBlockChange::run @ 0x1000E15CF */
    void run() override;

    /* FUNC: GameCommandBlockChange::postUnpack @ 0x1000E1545 */
    void postUnpack() override;

    /* FUNC: GameCommandBlockChange::done @ 0x1000E18A0 */
    bool isDone() const override;

    /* FUNC: GameCommandBlockChange::collection @ 0x1000E152F */
    bool isCollection() const override { return true; }

    const char* getDataDescriptor() const override { return "NNN[Nx][Nx][Nx]"; }

private:
    size_t _blockCount;    // GameCommandBlockChange::blockCount @ 0x100312EE8
    size_t _currentBlock;  // GameCommandBlockChange::currentBlock @ 0x100312EF0
};

}  // namespace opendw

#endif  // __GAME_COMMAND_BLOCK_CHANGE_H__

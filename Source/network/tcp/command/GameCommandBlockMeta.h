#ifndef __GAME_COMMAND_BLOCK_META_H__
#define __GAME_COMMAND_BLOCK_META_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandBlockMeta : GameCommand @ 0x10031A5A0
 */
class GameCommandBlockMeta : public GameCommand
{
public:
    /* FUNC: GameCommandBlockMeta::run @ 0x1000FBBA1 */
    void run() override;

    /* FUNC: GameCommandBlockMeta::collection @ 0x1000FBB89 */
    bool isCollection() const override { return true; }

    /* FUNC: GameCommandBlockMeta::validArrayDataDescriptor @ 0x1000FBB94 */
    const char* getDataDescriptor() const override { return "NN[Dx]"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_BLOCK_META_H__

#ifndef __GAME_COMMAND_EFFECT_H__
#define __GAME_COMMAND_EFFECT_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandEffect : GameCommand @ 0x10031AF00
 */
class GameCommandEffect : public GameCommand
{
public:
    /* FUNC: GameCommandEffect::run @ 0x10011DFA0 */
    void run() override;

    /* FUNC: GameCommandEffect::validArrayDataDescriptor @ 0x10011DF93 */
    const char* getDataDescriptor() const override { return "NNS."; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_EFFECT_H__

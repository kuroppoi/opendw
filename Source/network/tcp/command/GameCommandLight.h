#ifndef __GAME_COMMAND_LIGHT_H__
#define __GAME_COMMAND_LIGHT_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandLight : GameCommand @ 0x100318D40
 */
class GameCommandLight : public GameCommand
{
public:
    /* FUNC: GameCommandLight::run @ 0x1000D07FD */
    void run() override;

    /* FUNC: GameCommandLight::collection @ 0x1000D07DA */
    bool isCollection() const override { return true; }

    /* FUNC: GameCommandLight::validArrayDataDescriptor @ 0x1000D07F0 */
    const char* getDataDescriptor() const override { return "NNNA"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_LIGHT_H__

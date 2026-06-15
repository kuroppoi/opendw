#ifndef __GAME_COMMAND_SKILL_H__
#define __GAME_COMMAND_SKILL_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandSkill : GameCommand @ 0x10031B130
 */
class GameCommandSkill : public GameCommand
{
public:
    /* FUNC: GameCommandSkill::run @ 0x100121B5A */
    void run() override;

    /* FUNC: GameCommandSkill::collection @ 0x100121B42 */
    bool isCollection() const override { return true; }

    /* FUNC: GameCommandSkill::validArrayDataDescriptor @ 0x100121B4D */
    const char* getDataDescriptor() const override { return "SN"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_SKILL_H__

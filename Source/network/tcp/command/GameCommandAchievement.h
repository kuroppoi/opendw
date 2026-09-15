#ifndef __GAME_COMMAND_ACHIEVEMENT_H__
#define __GAME_COMMAND_ACHIEVEMENT_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandAchievement : GameCommand @ 0x10031AEB0
 */
class GameCommandAchievement : public GameCommand
{
public:
    /* FUNC: GameCommandAchievement::run @ 0x10011C525 */
    void run() override;

    /* FUNC: GameCommandAchievement::collection @ 0x10011C50D */
    bool isCollection() const override { return true; }

    /* FUNC: GameCommandAchievement::validArrayDataDescriptor @ 0x10011C518 */
    const char* getDataDescriptor() const override { return "SN"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_ACHIEVEMENT_H__

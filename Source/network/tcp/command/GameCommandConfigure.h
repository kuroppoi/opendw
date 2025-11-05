#ifndef __GAME_COMMAND_CONFIGURE_H__
#define __GAME_COMMAND_CONFIGURE_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandConfigure : GameCommand @ 0x10031BFB8
 */
class GameCommandConfigure : public GameCommand
{
public:
    /* FUNC: GameCommandConfigure::initWithData: @ 0x10013CD54 */
    void initWithData(const uint8_t* data, size_t length) override;

    /* FUNC: GameCommandConfigure::run @ 0x10013CDCA */
    void run() override;

    /* FUNC: GameCommandConfigure::compressed @ 0x10013CD3E */
    bool isCompressed() const override { return true; }

    const char* getDataDescriptor() const override { return "[SN]DDD"; }
};

}  // namespace opendw

#endif  // __GAME_COMMAND_CONFIGURE_H__

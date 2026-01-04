#ifndef __GAME_COMMAND_BLOCKS_H__
#define __GAME_COMMAND_BLOCKS_H__

#include "network/tcp/command/GameCommand.h"

namespace opendw
{

/*
 * CLASS: GameCommandBlocks : GameCommand @ 0x100318F70
 */
class GameCommandBlocks : public GameCommand
{
public:
    /* FUNC: GameCommandBlocks::run @ 0x1000E1952 */
    void run() override;

    /* FUNC: GameCommandBlocks::postUnpack @ 0x1000E18F3 */
    void postUnpack() override;

    /* FUNC: GameCommandBlocks::done @ 0x1000E1BDD */
    bool isDone() const override;

    /* FUNC: GameCommandBlocks::collection @ 0x1000E18D2 */
    bool isCollection() const override { return true; }

    /* FUNC: GameCommandBlocks::compressed @ 0x1000E18DD */
    bool isCompressed() const override { return true; }

    const char* getDataDescriptor() const override { return "NNNNA"; }

private:
    size_t _chunkCount   = 0;  // GameCommandBlocks::chunkCount @ 0x100312F00
    size_t _currentChunk = 0;  // GameCommandBlocks::currentChunk @ 0x100312F08
};

}  // namespace opendw

#endif  // __GAME_COMMAND_BLOCKS_H__

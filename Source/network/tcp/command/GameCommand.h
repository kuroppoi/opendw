#ifndef __GAME_COMMAND_H__
#define __GAME_COMMAND_H__

#include "axmol.h"

namespace opendw
{

/*
 * CLASS: GameCommand : NSOperation @ 0x100318048
 */
class GameCommand : public ax::Object
{
public:
    enum class Ident : uint8_t
    {
        CONFIGURE       = 2,
        BLOCKS          = 3,
        PLAYER_POSITION = 5,
        BLOCK_CHANGE    = 9,
        ZONE_STATUS     = 17,
        NOTIFICATION    = 33,
        KICK            = 255
    };

    /* FUNC: GameCommand::commandClassForCode: @ 0x1000A16AC */
    static GameCommand* createFromIdent(Ident ident);

    /* FUNC: GameCommand::initWithCommandData: @ 0x1000A1A9D */
    virtual void initWithData(const uint8_t* data, size_t length);

    /* FUNC: GameCommand::valid @ 0x1000A1FF2 */
    bool validate();

    /* FUNC: GameCommand::addErrorAndNotify: @ 0x1000A246B */
    void addError(const std::string& error, bool notify = false);

    /* FUNC: GameCommand::errors @ 0x1000A2577 */
    const std::vector<std::string>& getErrors() const { return _errors; }

    /* FUNC: GameCommand::run @ 0x1000A1EF9 */
    virtual void run() = 0;

    /* FUNC: GameCommand::postUnpack @ 0x1000A1E82 */
    virtual void postUnpack() {}

    /* FUNC: GameCommand::done @ 0x1000A1FE1 */
    virtual bool isDone() const { return true; }

    /* FUNC: GameCommand::highPriorty @ 0x1000A1E88 */
    virtual bool isHighPriorty() const { return false; }

    /* FUNC: GameCommand::collection @ 0x1000A1BFA */
    virtual bool isCollection() const { return false; }

    /* FUNC: GameCommand::compressed @ 0x1000A1C02 */
    virtual bool isCompressed() const { return false; }

    /* FUNC: GameCommand::validArrayDataDescriptor @ 0x1000A22AA */
    virtual const char* getDataDescriptor() const { return ""; }

protected:
    ax::ValueVector _data;             // GameCommand::data @ 0x100321618
    std::vector<std::string> _errors;  // GameCommand::errors @ 0x100312620
};

}  // namespace opendw

#endif  // __GAME_COMMAND_H__

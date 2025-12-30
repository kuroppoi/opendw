#include "GameCommand.h"

#include "msgpack/MessagePack.h"
#include "network/tcp/command/GameCommandBlockChange.h"
#include "network/tcp/command/GameCommandBlocks.h"
#include "network/tcp/command/GameCommandConfigure.h"
#include "network/tcp/command/GameCommandEntityChange.h"
#include "network/tcp/command/GameCommandEntityPosition.h"
#include "network/tcp/command/GameCommandEntityStatus.h"
#include "network/tcp/command/GameCommandKick.h"
#include "network/tcp/command/GameCommandNotification.h"
#include "network/tcp/command/GameCommandPlayerPosition.h"
#include "network/tcp/command/GameCommandZoneSearch.h"
#include "network/tcp/command/GameCommandZoneStatus.h"
#include "util/Validation.h"

USING_NS_AX;

namespace opendw
{

GameCommand* GameCommand::createFromIdent(Ident ident)
{
    switch (ident)
    {
    case Ident::CONFIGURE:
        return new GameCommandConfigure();
    case Ident::BLOCKS:
        return new GameCommandBlocks();
    case Ident::PLAYER_POSITION:
        return new GameCommandPlayerPosition();
    case Ident::ENTITY_POSITION:
        return new GameCommandEntityPosition();
    case Ident::ENTITY_STATUS:
        return new GameCommandEntityStatus();
    case Ident::ENTITY_CHANGE:
        return new GameCommandEntityChange();
    case Ident::BLOCK_CHANGE:
        return new GameCommandBlockChange();
    case Ident::ZONE_STATUS:
        return new GameCommandZoneStatus();
    case Ident::ZONE_SEARCH:
        return new GameCommandZoneSearch();
    case Ident::NOTIFICATION:
        return new GameCommandNotification();
    case Ident::KICK:
        return new GameCommandKick();
    default:
        return nullptr;
    }
}

void GameCommand::initWithData(const uint8_t* data, size_t length)
{
    try
    {
        msgpack::MessagePackParser parser(data, length);
        _data = parser.unpackArray();
    }
    catch (msgpack::ParseException& ex)
    {
        addError("Message unpack failed", true);
    }

    postUnpack();
}

bool GameCommand::validate()
{
    if (isCollection())
    {
        // Validate each element in the collection
        for (size_t i = 0; i < _data.size(); i++)
        {
            auto& element = _data[i];

            // Check if element is an array (collections elements have to be arrays)
            if (element.getType() == Value::Type::VECTOR)
            {
                auto& array     = element.asValueVector();
                auto descriptor = validation::createArrayDescriptor(array);

                // Check if element matches the command data descriptor
                if (!validation::validateDescriptor(descriptor, getDataDescriptor()))
                {
                    addError(std::format("Collection data at index {} does not match descriptor:", i, descriptor));
                }
            }
            else
            {
                auto descriptor = validation::getValueDescriptor(element);
                addError(std::format("Collection data at index {} is not an array (is '{}')", i, descriptor));
            }
        }
    }
    else
    {
        auto descriptor = validation::createArrayDescriptor(_data);

        // Check if data matches the command descriptor
        if (!validation::validateDescriptor(descriptor, getDataDescriptor()))
        {
            addError(std::format("Data does not match descriptor: {}", descriptor));
        }
    }

    return _errors.empty();
}

void GameCommand::addError(const std::string& error, bool notify)
{
    _errors.push_back(error);

    if (notify)
    {
        AXLOGE("[GameCommand] CRITICAL ERROR: {}", error);
    }
}

}  // namespace opendw

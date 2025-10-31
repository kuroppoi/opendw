#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "axmol.h"

#include "msgpack/MessagePackPacker.h"

namespace opendw
{

struct Message
{
    enum class Ident : uint8_t
    {
        AUTHENTICATE  = 1,
        MOVE          = 5,
        BLOCKS        = 16,
        BLOCKS_IGNORE = 25
    } ident;

    virtual void pack(msgpack::MessagePackPacker& packer) const = 0;
};

struct AuthenticateMessage : Message
{
    std::string version;
    std::string name;
    std::string token;
    bool initial;

    AuthenticateMessage(const std::string& _version,
                        const std::string& _name,
                        const std::string& _token,
                        bool _initial = true)
        : version(_version), name(_name), token(_token), initial(_initial)
    {
        ident = Ident::AUTHENTICATE;
    }

    void pack(msgpack::MessagePackPacker& packer) const override
    {
        packer.packArrayStart(4);
        packer.packString(version);
        packer.packString(name);
        packer.packString(token);

        ax::ValueMap details;
        details["initial"] = initial;
        packer.packMap(details);
    }
};

struct MoveMessage : Message
{
    ax::Point position;
    ax::Vec2 velocity;
    uint8_t direction;
    ax::Point target;
    uint32_t animation;

    MoveMessage(const ax::Point& _position,
                const ax::Vec2& _velocity,
                uint8_t _direction,
                const ax::Point& _target,
                uint32_t _animation)
        : position(_position), velocity(_velocity), direction(_direction), target(_target), animation(_animation)
    {
        ident = Ident::MOVE;
    }

    void pack(msgpack::MessagePackPacker& packer) const override
    {
        packer.packArrayStart(8);
        packer.packFloat(position.x);
        packer.packFloat(position.y);
        packer.packFloat(velocity.x);
        packer.packFloat(velocity.y);
        packer.packUInt8(direction);
        packer.packFloat(target.x);
        packer.packFloat(target.y);
        packer.packUInt32(animation);
    }
};

struct BlocksMessage : Message
{
    std::span<uint32_t> chunks;

    BlocksMessage(const std::span<uint32_t>& _chunks) : chunks(_chunks) { ident = Ident::BLOCKS; }

    void pack(msgpack::MessagePackPacker& packer) const override
    {
        packer.packArrayStart(1);
        packer.packArrayStart(chunks.size());

        for (auto chunk : chunks)
        {
            packer.packUInt32(chunk);
        }
    }
};

struct BlocksIgnoreMessage : BlocksMessage
{
    BlocksIgnoreMessage(const std::span<uint32_t>& _chunks) : BlocksMessage(_chunks) { ident = Ident::BLOCKS_IGNORE; }
};

}  // namespace opendw

#endif  // __MESSAGE_H__

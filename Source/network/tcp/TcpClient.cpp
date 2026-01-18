#include "TcpClient.h"

#include "msgpack/MessagePack.h"
#include "network/tcp/command/GameCommand.h"
#include "network/tcp/MessageIdent.h"
#include "util/ArrayUtil.h"
#include "util/MapUtil.h"
#include "util/StringUtil.h"
#include "GameManager.h"

#define READ_BUFFER_SIZE    512 * 1024       // 512 KB
#define INFLATE_BUFFER_SIZE 4 * 1024 * 1024  // 4 MB
#define CHANNEL_INDEX       0
#define HEADER_LENGTH       5

USING_NS_AX;
using namespace yasio;

namespace opendw
{

TcpClient::~TcpClient()
{
    AX_SAFE_DELETE(_service);
    AX_SAFE_DELETE_ARRAY(_readBuffer);
    AX_SAFE_DELETE_ARRAY(_inflateBuffer);
}

TcpClient::TcpClient() : Object()
{
    _readBuffer    = new uint8_t[READ_BUFFER_SIZE];
    _inflateBuffer = new uint8_t[INFLATE_BUFFER_SIZE];
}

void TcpClient::connect(const char* address, uint16_t port)
{
    stop();
    AXLOGI("[TcpClient] Connecting to {}:{}", address, port);
    _service = new io_service({address, port});
    _service->set_option(YOPT_S_NO_DISPATCH, 1);
    _service->start([&](event_ptr&& event) {
        switch (event->kind())
        {
        case YEK_ON_PACKET:
            onPacket(event);
            break;
        case YEK_ON_OPEN:
            if (event->status() == 0)
            {
                onOpen(event);
            }
            else
            {
                AXLOGI("[TcpClient] Connection failed");
            }
            break;
        case YEK_ON_CLOSE:
            onClose(event);
            break;
        };
    });
    _service->open(CHANNEL_INDEX, YCK_TCP_CLIENT);
}

void TcpClient::stop()
{
    if (_service)
    {
        if (_service->is_running())
        {
            _service->close(CHANNEL_INDEX);
            _service->stop();
        }

        _service = nullptr;
    }
}

void TcpClient::dispatch()
{
    if (_service)
    {
        _service->dispatch();
    }
}

void TcpClient::sendMessage(MessageIdent ident, const ValueVector& data)
{
    msgpack::MessagePackPacker packer;
    packer.packArray(data);
    sendMessage(static_cast<uint8_t>(ident), packer.getOutput());
}

void TcpClient::sendMessage(uint8_t ident, const std::vector<uint8_t>& data)
{
    if (!_service || !_transport)
    {
        AXLOGW("[TcpClient] Attempted to send message while channel is closed");
        return;
    }

    auto length = data.size();
    obstream obs(HEADER_LENGTH + length);
    obs.write_byte(ident);
    obs.write_byte(length & 0xFF);
    obs.write_byte((length >> 8) & 0xFF);
    obs.write_byte((length >> 16) & 0xFF);
    obs.write_byte((length >> 24) & 0xFF);

    for (auto byte : data)
    {
        obs.write_byte(byte);
    }

    _service->write(_transport, std::move(obs.buffer()));
}

void TcpClient::processPacket(uint8_t ident, uint8_t* payload, uint32_t length)
{
    AXLOGD("[TcpClient] Received command: {}, len: {}", static_cast<int>(ident), length);
    auto command = GameCommand::createFromIdent(static_cast<GameCommand::Ident>(ident));

    if (!command)
    {
        AXLOGW("[TcpClient] Received unknown command: {}", static_cast<int>(ident));
        return;
    }

    command->autorelease();

    // Inflate payload if command is expected to be compressed
    if (command->isCompressed())
    {
        auto buf = ZipUtils::decompressGZ(payload, length);

        if (buf.length() > INFLATE_BUFFER_SIZE)
        {
            AXLOGW("[TcpClient] Uncompressed payload length ({}) exceeds buffer size!", buf.length());
            return;
        }

        std::copy(buf.begin(), buf.end(), _inflateBuffer);
        payload = _inflateBuffer;
        length  = buf.length();
    }

    command->initWithData(payload, length);

    // Validate unpacked command data
    if (!command->validate())
    {
        auto& errors = command->getErrors();
        auto name    = static_cast<int>(ident);  // TODO: use class name for easier debugging
        AXLOGE("------------------\nCommand error in {}: {}\n------------------\n", name,
               string_util::join(errors, "\n"));
    }
    else
    {
        GameManager::getInstance()->enqueueCommand(command);
    }
}

void TcpClient::onPacket(event_ptr& event)
{
    const auto& packet = std::move(event->packet());
    auto packetSize    = packet.size();

    if (_bytesRead + packetSize > READ_BUFFER_SIZE)
    {
        AXLOGE("[TcpClient] Read buffer limit reached!");
        _bytesRead     = 0;
        _header.ident  = 0;
        _header.length = 0;
        return;
    }

    std::copy(packet.begin(), packet.end(), _readBuffer + _bytesRead);
    _bytesRead += packetSize;

    while (true)
    {
        // Try to read next packet header
        // TODO: invalid data might make it wait forever (or at least until the read buffer is full)
        if (!_waitingForPayload && _bytesRead >= HEADER_LENGTH)
        {
            _header.ident  = _readBuffer[0];
            _header.length = _readBuffer[1] | (_readBuffer[2] << 8) | (_readBuffer[3] << 16) | (_readBuffer[4] << 24);
            _waitingForPayload = true;
        }

        // Try to read and process packet payload
        if (_waitingForPayload && _bytesRead >= HEADER_LENGTH + _header.length)
        {
            auto payloadStart = _readBuffer + HEADER_LENGTH;
            auto payloadEnd   = payloadStart + _header.length;
            processPacket(_header.ident, payloadStart, _header.length);
            _bytesRead -= HEADER_LENGTH;
            _bytesRead -= _header.length;
            std::copy(payloadEnd, payloadEnd + _bytesRead, _readBuffer);
            _waitingForPayload = false;
        }
        else
        {
            break;
        }
    }
}

void TcpClient::onOpen(event_ptr& event)
{
    AXLOGI("[TcpClient] Channel opened!");
    _open        = true;
    _transport   = event->transport();
    auto game    = GameManager::getInstance();
    auto user    = game->getCurrentUser();
    auto details = map_util::mapOf("initial", game->getConfig() == nullptr);
    sendMessage(MessageIdent::AUTHENTICATE, GAME_VERSION, user.username, user.token, details);
}

void TcpClient::onClose(event_ptr& event)
{
    AXLOGI("[TcpClient] Channel closed!");
    _open      = false;
    _transport = nullptr;
    _bytesRead = 0;
    GameManager::getInstance()->onDisconnected();
}

}  // namespace opendw

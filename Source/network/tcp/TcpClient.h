#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "axmol.h"
#include "yasio/yasio.hpp"

#define READ_BUFFER_SIZE    512 * 1024       // 512 KB
#define INFLATE_BUFFER_SIZE 4 * 1024 * 1024  // 4 MB

namespace opendw
{

struct Message;

class TcpClient : public ax::Object
{
public:
    ~TcpClient() override;

    void connect(const char* address, uint16_t port);
    void stop();

    void dispatch();

    void sendMessage(const Message& message);
    void sendMessage(uint8_t ident, const std::vector<uint8_t>& data);

    void processPacket(uint8_t ident, uint8_t* payload, uint32_t length);

    void onPacket(yasio::event_ptr& event);
    void onOpen(yasio::event_ptr& event);
    void onClose(yasio::event_ptr& event);

private:
    struct PacketHeader
    {
        uint8_t ident;
        uint32_t length;  // Payload length
    } _header;

    yasio::io_service* _service          = nullptr;
    yasio::transport_handle_t _transport = nullptr;
    uint8_t _readBuffer[READ_BUFFER_SIZE];
    uint8_t _inflateBuffer[INFLATE_BUFFER_SIZE];
    size_t _bytesRead       = 0;
    bool _waitingForPayload = false;
};

}  // namespace opendw

#endif  // __TCP_CLIENT_H__

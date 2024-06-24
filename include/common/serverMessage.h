#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include "message.h"

#include <cstddef>
#include <memory>

enum class ServerMessageType : uint8_t{
    MessageMessage,
    QuitMessage,
    LoginResultMessage,
    NotPrivilegedMessage,
    NUM_ELEMENTS
};

class ServerMessageHandler;

class ServerMessage : public Message<ServerMessageType>{
public:
    using MessagePointer = std::unique_ptr<ServerMessage>;

    virtual ~ServerMessage() = default;

    virtual void dispatch(ServerMessageHandler& handler) = 0;

    static MessageErrorStatus peek(const std::byte *buf, std::size_t size, MessageType &type_ret);

    static MessagePointer CreateMessage(MessageType type);

    static MessagePointer CreateMessage(const std::byte *buf, std::size_t size);
};

namespace ServerMessages{
    class MessageMessage;
    class QuitMessage;
    class LoginResultMessage;
    class NotPrivilegedMessage;
}

#endif
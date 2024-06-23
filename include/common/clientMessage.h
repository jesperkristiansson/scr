#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H

#include "message.h"

#include <cstddef>
#include <memory>

enum class ClientMessageType : uint8_t{
    JoinMessage,
    MessageMessage,
    QuitMessage,
    LoginMessage,
    NUM_ELEMENTS
};

class User;
class ClientMessageHandler;

class ClientMessage : public Message<ClientMessageType>{
public:
    using MessagePointer = std::unique_ptr<ClientMessage>;

    virtual void dispatch(ClientMessageHandler& handler, User &arg) = 0;

    static MessageErrorStatus peek(const std::byte *buf, std::size_t size, MessageType &type_ret);

    static MessagePointer CreateMessage(MessageType type);

    static MessagePointer CreateMessage(const std::byte *buf, std::size_t size);
};


namespace ClientMessages{
    class JoinMessage;
    class MessageMessage;
    class QuitMessage;
    class LoginMessage;
}

#endif
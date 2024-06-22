#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H

#include "message.h"

#include <cstddef>
#include <memory>

enum class MessageType : uint8_t{
    JoinMessage,
    MessageMessage,
    QuitMessage,
    NUM_ELEMENTS
};

class User;

class Message : public M<MessageType, User>{
public:
    using MessagePointer = std::unique_ptr<Message>;

    static MessageErrorStatus peek(const std::byte *buf, std::size_t size, MessageType &type_ret);

    static MessagePointer CreateMessage(MessageType type);

    static MessagePointer CreateMessage(const std::byte *buf, std::size_t size);
};

using MessagePointer = std::unique_ptr<Message>;

class JoinMessage;
class MessageMessage;
class QuitMessage;

#endif
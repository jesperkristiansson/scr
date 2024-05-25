#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <cstddef>
#include <memory>

enum class MessageErrorStatus{
    Success,
    NotEnoughData,
    InvalidData,
    Overflow,
    Other
};

class Handler;
class User;

class Message{
public:
    virtual void dispatch(Handler& handler, User &from) = 0;
    virtual std::size_t size() const = 0;
    virtual MessageErrorStatus read(const std::byte *&buf, std::size_t &size) = 0;
    virtual MessageErrorStatus write(std::byte *&buf, std::size_t &size) const = 0;
};

using MessagePointer = std::unique_ptr<Message>;

class JoinMessage;
class MessageMessage;
class QuitMessage;

#endif
#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <cstddef>

enum class MessageErrorStatus{
    Success,
    NotEnoughData,
    InvalidData,
    Overflow,
    Other
};

class Handler;


class Message{
public:
    virtual void dispatch(Handler& handler, int from) = 0;
    virtual std::size_t size() const = 0;
    virtual MessageErrorStatus read(const std::byte *&buf, std::size_t &size) = 0;
    virtual MessageErrorStatus write(std::byte *&buf, std::size_t &size) const = 0;
};

class JoinMessage;
class MessageMessage;
class QuitMessage;

#endif
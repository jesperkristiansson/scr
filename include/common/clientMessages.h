#ifndef CLIENT_MESSAGES_H
#define CLIENT_MESSAGES_H

#include "common/clientMessage.h"
#include "common/handler.h"

#include <cstdint>
#include <limits>

template<typename TDerived, MessageType MType>
class MessageBase : public ClientMessage{
public:
    virtual void dispatch(Handler& handler, User &from) override{
        handler.handle(dynamic_cast<TDerived&>(*this), from);
    }

    static MessagePointer create(){
        return MessagePointer(new TDerived);
    }

protected:
    MessageType type() const override{
        return MType;
    }
};

class JoinMessage : public MessageBase<JoinMessage, MessageType::JoinMessage>{
public:
    JoinMessage() {}
    JoinMessage(const std::string &str) : room_name(str) {}

    std::size_t size() const override{
        return sizeof(MessageType) + sizeof(uint16_t) + room_name.size();
    }

    MessageErrorStatus read(const std::byte *&buf, std::size_t &size) override{
        const std::byte *_buf = buf;
        std::size_t _size = size;

        MessageErrorStatus res;

        //verify type
        res = readType(_buf, _size, type());
        if(res != MessageErrorStatus::Success){
            return res;
        }

        //read string
        res = readString(_buf, _size, room_name);
        if(res != MessageErrorStatus::Success){
            return res;
        }

        buf = _buf;
        size = _size;

        return MessageErrorStatus::Success;
    }

    MessageErrorStatus write(std::byte *&buf, std::size_t &size) const override{
        std::byte *_buf = buf;
        std::size_t _size = size;

        MessageErrorStatus res;

        //verify validity
        res = writeType(_buf, _size, type());
        if(res != MessageErrorStatus::Success){
            return res;
        }

        res = writeString(_buf, _size, room_name);
        if(res != MessageErrorStatus::Success){
            return res;
        }

        buf = _buf;
        size = _size;

        return MessageErrorStatus::Success;
    }

    std::string room_name;
};

class MessageMessage : public MessageBase<MessageMessage, MessageType::MessageMessage>{
public:
    MessageMessage() {}
    MessageMessage(const std::string &str) : msg(str) {}

    std::size_t size() const override{
        return sizeof(MessageType) + sizeof(uint16_t) + msg.size();
    }

    MessageErrorStatus read(const std::byte *&buf, std::size_t &size) override{
        const std::byte *_buf = buf;
        std::size_t _size = size;

        MessageErrorStatus res;

        //verify type
        res = readType(_buf, _size, type());
        if(res != MessageErrorStatus::Success){
            return res;
        }

        //read string
        res = readString(_buf, _size, msg);
        if(res != MessageErrorStatus::Success){
            return res;
        }

        buf = _buf;
        size = _size;

        return MessageErrorStatus::Success;
    }

    MessageErrorStatus write(std::byte *&buf, std::size_t &size) const override {
        std::byte *_buf = buf;
        std::size_t _size = size;

        MessageErrorStatus res;

        //verify validity
        res = writeType(_buf, _size, type());
        if(res != MessageErrorStatus::Success){
            return res;
        }

        res = writeString(_buf, _size, msg);
        if(res != MessageErrorStatus::Success){
            return res;
        }

        buf = _buf;
        size = _size;

        return MessageErrorStatus::Success;
    }
    std::string msg;
};

class QuitMessage : public MessageBase<QuitMessage, MessageType::QuitMessage>{
    std::size_t size() const override{
        return sizeof(MessageType);
    }

    MessageErrorStatus read(const std::byte *&buf, std::size_t &size) override{
        const std::byte *_buf = buf;
        std::size_t _size = size;

        MessageErrorStatus res;

        //verify type
        res = readType(_buf, _size, type());
        if(res != MessageErrorStatus::Success){
            return res;
        }

        buf = _buf;
        size = _size;

        return MessageErrorStatus::Success;
    }

    MessageErrorStatus write(std::byte *&buf, std::size_t &size) const override {
        std::byte *_buf = buf;
        std::size_t _size = size;

        MessageErrorStatus res;

        //verify validity
        res = writeType(_buf, _size, type());
        if(res != MessageErrorStatus::Success){
            return res;
        }

        buf = _buf;
        size = _size;

        return MessageErrorStatus::Success;
    }
};

#endif

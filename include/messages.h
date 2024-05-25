#ifndef MESSAGES_H
#define MESSAGES_H

#include "message.h"
#include "handler.h"

#include <memory>
#include <cstdint>
#include <arpa/inet.h>
#include <cstring>

enum class MessageType : uint8_t{
    JoinMessage,
    MessageMessage,
    QuitMessage,
    NUM_ELEMENTS
};

using MessagePointer = std::unique_ptr<Message>;

template<typename TDerived, MessageType MType>
class MessageBase : public Message{
public:
    virtual void dispatch(Handler& handler, int from) override{
        handler.handle(dynamic_cast<TDerived&>(*this), from);
    }

    static MessagePointer create(){
        return MessagePointer(new TDerived);
    }

protected:
    MessageErrorStatus readType(const std::byte *&buf, std::size_t &size){
        if(size < sizeof(MessageType)){
            return MessageErrorStatus::NotEnoughData;
        }

        if(*reinterpret_cast<const MessageType*>(buf) != MessageType::JoinMessage){
            return MessageErrorStatus::InvalidData;
        }

        buf += sizeof(MessageType);
        size -= sizeof(MessageType);

        return MessageErrorStatus::Success;
    }

    MessageErrorStatus readUInt16(const std::byte *&buf, std::size_t &size, uint16_t &val_res){
        if(size < sizeof(val_res)){
            return MessageErrorStatus::NotEnoughData;
        }
        val_res = ntohs(*reinterpret_cast<const uint16_t*>(buf));
        buf += sizeof(val_res);
        size -= sizeof(val_res);

        return MessageErrorStatus::Success;
    }

    MessageErrorStatus readString(const std::byte *&buf, std::size_t &size, std::string &str){
        MessageErrorStatus res;

        uint16_t str_size;
        res = readUInt16(buf, size, str_size);
        if(res != MessageErrorStatus::Success){
            return res;
        }

        if(size < str_size){
            return MessageErrorStatus::NotEnoughData;
        }
        str = std::string(reinterpret_cast<const char*>(buf), str_size);
        buf += str_size;
        size -= str_size;

        return MessageErrorStatus::Success;
    }

    MessageErrorStatus writeType(std::byte *&buf, std::size_t &size) const {
        if(size < sizeof(MessageType)){
            return MessageErrorStatus::Overflow;
        }

        *reinterpret_cast<MessageType*>(buf) = MessageType::JoinMessage;
        buf += sizeof(MessageType);
        size -= sizeof(MessageType);

        return MessageErrorStatus::Success;
    }

    MessageErrorStatus writeUInt16(std::byte *&buf, std::size_t &size, uint16_t val) const {
        if(size < sizeof(val)){
            return MessageErrorStatus::Overflow;
        }

        if(std::numeric_limits<uint16_t>::max() < val){
            return MessageErrorStatus::Overflow;
        }

        *reinterpret_cast<uint16_t*>(buf) = val;
        buf += sizeof(uint16_t);
        size -= sizeof(uint16_t);

        return MessageErrorStatus::Success;
    }

    MessageErrorStatus writeBuffer(std::byte *&buf, std::size_t &size, const void *in_buf, std::size_t in_size) const {
        if(size < in_size){
            return MessageErrorStatus::Overflow;
        }

        memcpy(buf, in_buf, in_size);
        buf += in_size;
        size -= in_size;

        return MessageErrorStatus::Success;
    }

    MessageErrorStatus writeString(std::byte *&buf, std::size_t &size, const std::string &str) const {
        MessageErrorStatus res;

        res = writeUInt16(buf, size, static_cast<uint16_t>(str.size()));
        if(res != MessageErrorStatus::Success){
            return res;
        }

        res = writeBuffer(buf, size, str.data(), str.size());
        if(res != MessageErrorStatus::Success){
            return res;
        }

        return MessageErrorStatus::Success;
    }
};

class JoinMessage : public MessageBase<JoinMessage, MessageType::JoinMessage>{
public:
    std::size_t size() const override{
        return sizeof(MessageType) + sizeof(uint16_t) + room_name.size();
    }

    MessageErrorStatus read(const std::byte *&buf, std::size_t &size) override{
        const std::byte *_buf = buf;
        std::size_t _size = size;

        MessageErrorStatus res;

        //verify type
        res = readType(_buf, _size);
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
        res = writeType(_buf, _size);
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
    std::size_t size() const override{
        return sizeof(MessageType) + sizeof(uint16_t) + msg.size();
    }

    MessageErrorStatus read(const std::byte *&buf, std::size_t &size) override{
        const std::byte *_buf = buf;
        std::size_t _size = size;

        MessageErrorStatus res;

        //verify type
        res = readType(_buf, _size);
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
        res = writeType(_buf, _size);
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

class QuitMessage : public MessageBase<QuitMessage, MessageType::MessageMessage>{
    std::size_t size() const override{
        return sizeof(MessageType);
    }

    MessageErrorStatus read(const std::byte *&buf, std::size_t &size) override{
        const std::byte *_buf = buf;
        std::size_t _size = size;

        MessageErrorStatus res;

        //verify type
        res = readType(_buf, _size);
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
        res = writeType(_buf, _size);
        if(res != MessageErrorStatus::Success){
            return res;
        }

        buf = _buf;
        size = _size;

        return MessageErrorStatus::Success;
    }
};

MessageErrorStatus peek(const std::byte *buf, std::size_t size, MessageType &type_ret);

MessagePointer CreateMessage(MessageType type);

MessagePointer CreateMessage(const std::byte *buf, std::size_t size);

#endif
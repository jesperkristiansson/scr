#ifndef MESSAGES_H
#define MESSAGES_H

#include "common/message.h"
#include "common/handler.h"

#include <cstdint>
#include <arpa/inet.h>
#include <cstring>
#include <limits>

enum class MessageType : uint8_t{
    JoinMessage,
    MessageMessage,
    QuitMessage,
    NUM_ELEMENTS
};

template<typename TDerived, MessageType MType>
class MessageBase : public Message{
public:
    virtual void dispatch(Handler& handler, User &from) override{
        handler.handle(dynamic_cast<TDerived&>(*this), from);
    }

    static MessagePointer create(){
        return MessagePointer(new TDerived);
    }

protected:
    static MessageType type(){
        return MType;
    }

    static MessageErrorStatus readType(const std::byte *&buf, std::size_t &size, MessageType expected_type){
        if(size < sizeof(MessageType)){
            return MessageErrorStatus::NotEnoughData;
        }

        if(*reinterpret_cast<const MessageType*>(buf) != expected_type){
            return MessageErrorStatus::InvalidData;
        }

        buf += sizeof(MessageType);
        size -= sizeof(MessageType);

        return MessageErrorStatus::Success;
    }

    static MessageErrorStatus readUInt16(const std::byte *&buf, std::size_t &size, uint16_t &val_res){
        if(size < sizeof(val_res)){
            return MessageErrorStatus::NotEnoughData;
        }
        val_res = ntohs(*reinterpret_cast<const uint16_t*>(buf));
        buf += sizeof(val_res);
        size -= sizeof(val_res);

        return MessageErrorStatus::Success;
    }

    static MessageErrorStatus readString(const std::byte *&buf, std::size_t &size, std::string &str){
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

    static MessageErrorStatus writeType(std::byte *&buf, std::size_t &size, MessageType type){
        if(size < sizeof(MessageType)){
            return MessageErrorStatus::Overflow;
        }

        *reinterpret_cast<MessageType*>(buf) = type;
        buf += sizeof(MessageType);
        size -= sizeof(MessageType);

        return MessageErrorStatus::Success;
    }

    static MessageErrorStatus writeUInt16(std::byte *&buf, std::size_t &size, uint16_t val){
        if(size < sizeof(val)){
            return MessageErrorStatus::Overflow;
        }

        if(std::numeric_limits<uint16_t>::max() < val){
            return MessageErrorStatus::Overflow;
        }

        *reinterpret_cast<uint16_t*>(buf) = htons(val);
        buf += sizeof(uint16_t);
        size -= sizeof(uint16_t);

        return MessageErrorStatus::Success;
    }

    static MessageErrorStatus writeBuffer(std::byte *&buf, std::size_t &size, const void *in_buf, std::size_t in_size){
        if(size < in_size){
            return MessageErrorStatus::Overflow;
        }

        memcpy(buf, in_buf, in_size);
        buf += in_size;
        size -= in_size;

        return MessageErrorStatus::Success;
    }

    static MessageErrorStatus writeString(std::byte *&buf, std::size_t &size, const std::string &str){
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

MessageErrorStatus peek(const std::byte *buf, std::size_t size, MessageType &type_ret);

MessagePointer CreateMessage(MessageType type);

MessagePointer CreateMessage(const std::byte *buf, std::size_t size);

#endif

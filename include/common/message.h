#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstddef>
#include <memory>

#include <arpa/inet.h>
#include <cstring>

enum class MessageErrorStatus{
    Success,
    NotEnoughData,
    InvalidData,
    Overflow,
    Other
};

class Handler;

template<typename TMessageType, typename THandleArgType>
class M{
public:
    using MessageType = TMessageType;
    using MPointer = std::unique_ptr<M<TMessageType, THandleArgType>>;

    virtual std::size_t size() const = 0;
    virtual void dispatch(Handler& handler, THandleArgType &arg) = 0;
    virtual MessageErrorStatus read(const std::byte *&buf, std::size_t &size) = 0;
    virtual MessageErrorStatus write(std::byte *&buf, std::size_t &size) const = 0;

protected:
    virtual MessageType type() const;

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

#endif
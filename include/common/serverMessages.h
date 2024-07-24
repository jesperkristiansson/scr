#ifndef SERVER_MESSAGES_H
#define SERVER_MESSAGES_H

#include "common/serverMessage.h"
#include "common/serverMessageHandler.h"

#include <cstdint>
#include <limits>

namespace ServerMessages{

    template<typename TDerived, ServerMessage::MessageType MType>
    class MessageBase : public ServerMessage{
    public:
        virtual void dispatch(ServerMessageHandler& handler) override{
            handler.handle(dynamic_cast<TDerived&>(*this));
        }

        static MessagePointer create(){
            return MessagePointer(new TDerived);
        }

    protected:
        MessageType type() const override{
            return MType;
        }
    };

    class MessageMessage : public MessageBase<MessageMessage, ServerMessage::MessageType::MessageMessage>{
    public:
        MessageMessage() {}
        MessageMessage(uint32_t time, const std::string &from_name, const std::string &str) : time(time), from_name(from_name), msg(str) {}

        std::size_t size() const override{
            return sizeof(MessageType) + sizeof(time) + sizeof(uint16_t) + from_name.size() + sizeof(uint16_t) + msg.size();
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

            //read time
            res = readUInt32(_buf, _size, time);
            if(res != MessageErrorStatus::Success){
                return res;
            }

            //read name
            res = readString(_buf, _size, from_name);
            if(res != MessageErrorStatus::Success){
                return res;
            }

            //read msg
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

            res = writeType(_buf, _size, type());
            if(res != MessageErrorStatus::Success){
                return res;
            }

            //write time
            res = writeUInt32(_buf, _size, time);
            if(res != MessageErrorStatus::Success){
                return res;
            }

            //write name
            res = writeString(_buf, _size, from_name);
            if(res != MessageErrorStatus::Success){
                return res;
            }

            //write msg
            res = writeString(_buf, _size, msg);
            if(res != MessageErrorStatus::Success){
                return res;
            }

            buf = _buf;
            size = _size;

            return MessageErrorStatus::Success;
        }
        uint32_t time;
        std::string from_name;
        std::string msg;
    };

    class QuitMessage : public MessageBase<QuitMessage, ServerMessage::MessageType::QuitMessage>{
        public:
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

    class LoginResultMessage : public MessageBase<LoginResultMessage, ServerMessage::MessageType::LoginResultMessage>{
    public:
        LoginResultMessage() {}
        LoginResultMessage(uint16_t result) : result(result) {}

        std::size_t size() const override{
            return sizeof(MessageType) + sizeof(uint16_t);
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

            //read result
            res = readUInt16(_buf, _size, result);
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

            res = writeType(_buf, _size, type());
            if(res != MessageErrorStatus::Success){
                return res;
            }

            //write name
            res = writeUInt16(_buf, _size, result);
            if(res != MessageErrorStatus::Success){
                return res;
            }

            buf = _buf;
            size = _size;

            return MessageErrorStatus::Success;
        }
        uint16_t result;
    };

    class NotPrivilegedMessage : public MessageBase<NotPrivilegedMessage, ServerMessage::MessageType::NotPrivilegedMessage>{
    public:
        NotPrivilegedMessage() {}

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

            res = writeType(_buf, _size, type());
            if(res != MessageErrorStatus::Success){
                return res;
            }

            buf = _buf;
            size = _size;

            return MessageErrorStatus::Success;
        }
    };
}

#endif

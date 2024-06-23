#ifndef CLIENT_MESSAGES_H
#define CLIENT_MESSAGES_H

#include "common/clientMessage.h"
#include "common/clientMessageHandler.h"

#include <cstdint>
#include <limits>

namespace ClientMessages{

    template<typename TDerived, ClientMessage::MessageType MType>
    class MessageBase : public ClientMessage{
    public:
        virtual void dispatch(ClientMessageHandler& handler, User &from) override{
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

    class JoinMessage : public MessageBase<JoinMessage, ClientMessage::MessageType::JoinMessage>{
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

    class MessageMessage : public MessageBase<MessageMessage, ClientMessage::MessageType::MessageMessage>{
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

    class QuitMessage : public MessageBase<QuitMessage, ClientMessage::MessageType::QuitMessage>{
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

    class LoginMessage : public MessageBase<LoginMessage, ClientMessage::MessageType::LoginMessage>{
    public:
        LoginMessage() {}
        LoginMessage(const std::string &name, const std::string &passwd) : name(name), passwd(passwd) {}

        std::size_t size() const override{
            return sizeof(MessageType) + sizeof(uint16_t) + name.size() + sizeof(uint16_t) + passwd.size();
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

            //read name
            res = readString(_buf, _size, name);
            if(res != MessageErrorStatus::Success){
                return res;
            }

            //read passwd
            res = readString(_buf, _size, passwd);
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

            res = writeString(_buf, _size, name);
            if(res != MessageErrorStatus::Success){
                return res;
            }

            res = writeString(_buf, _size, passwd);
            if(res != MessageErrorStatus::Success){
                return res;
            }

            buf = _buf;
            size = _size;

            return MessageErrorStatus::Success;
        }
        std::string name;
        std::string passwd;
    };

}

#endif

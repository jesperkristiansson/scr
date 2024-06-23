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

    class QuitMessage : public MessageBase<QuitMessage, ServerMessage::MessageType::QuitMessage>{
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

}

#endif

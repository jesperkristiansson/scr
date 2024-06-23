#ifndef CLIENT_MESSAGE_HANDLER_H
#define CLIENT_MESSAGE_HANDLER_H

#include "common/clientMessage.h"

class ClientMessageHandler{
    public:
        virtual void handle(ClientMessages::JoinMessage& msg, User &from) = 0;
        virtual void handle(ClientMessages::MessageMessage &msg, User &from) = 0;
        virtual void handle(ClientMessages::QuitMessage &msg, User &from) = 0;
        virtual void handle(ClientMessage& msg, User &from) = 0;
    protected:
        ClientMessageHandler() = default;
};

#endif
#ifndef SERVER_MESSAGE_HANDLER_H
#define SERVER_MESSAGE_HANDLER_H

#include "common/serverMessage.h"

class ServerMessageHandler{
    public:
        virtual void handle(ServerMessages::MessageMessage &msg) = 0;
        virtual void handle(ServerMessages::QuitMessage &msg) = 0;
        virtual void handle(ServerMessages::LoginResultMessage& msg) = 0;
        virtual void handle(ServerMessage& msg) = 0;
    protected:
        ServerMessageHandler() = default;
};

#endif
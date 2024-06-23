#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include "server/user.h"

#include "common/clientMessageHandler.h"
#include "common/clientMessages.h"

class Server;

class ServerHandler : public ClientMessageHandler{
public:
    ServerHandler(Server *s) : server{s} {}
    void setServer(Server *s){server = s;}
    void handle(ClientMessages::JoinMessage& msg, User &from);
    void handle(ClientMessages::MessageMessage &msg, User &from);
    void handle(ClientMessages::QuitMessage &msg, User &from);
    void handle(ClientMessages::LoginMessage &msg, User &from);
    void handle(ClientMessage& msg, User &from);
private:
    Server *server;
};

#endif
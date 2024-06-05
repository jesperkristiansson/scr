#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include "server/user.h"

#include "common/handler.h"
#include "common/messages.h"

class Server;

class ServerHandler : public Handler{
public:
    ServerHandler(Server *s) : server{s} {}
    void setServer(Server *s){server = s;}
    void handle(JoinMessage& msg, User &from);
    void handle(MessageMessage &msg, User &from);
    void handle(QuitMessage &msg, User &from);
    void handle(Message& msg, User &from);
private:
    Server *server;
};

#endif
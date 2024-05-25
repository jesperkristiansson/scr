#ifndef SERVER_H
#define SERVER_H

#include "house.h"
#include "message.h"
#include "handler.h"

#include <cstdint>
#include <vector>
#include <poll.h>

class Server{
public:
    Server(int server_fd) : server_fd{server_fd}, poll_items{}, house{}, handler(*this) {}
    ~Server();
    void run();

private:
    int server_fd;
    std::vector<struct pollfd> poll_items;
    House house;

    //inner class
    class ServerHandler : Handler{
    public:
        ServerHandler(Server& s) : server{s} {}
        void handle(JoinMessage& msg, int from);
        void handle(MessageMessage &msg, int from);
        void handle(QuitMessage &msg, int from);
        void handle(Message& msg, int from);
    private:
        Server& server;
    };
    ServerHandler handler;

    bool handle_poll_event(struct pollfd& item);
    bool accept_connections();
};

#endif
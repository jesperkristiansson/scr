#ifndef SERVER_H
#define SERVER_H

#include "house.h"
#include "message.h"
#include "handler.h"
#include "user.h"

#include <cstdint>
#include <vector>
#include <poll.h>
#include <unordered_map>

class Server{
public:
    Server(int server_fd) : server_fd{server_fd}, poll_items{}, house{}, handler(*this) {}
    ~Server();
    void run();

private:
    int server_fd;
    std::vector<struct pollfd> poll_items;
    House house;
    std::unordered_map<int, User> users;

    //inner class
    class ServerHandler : public Handler{
    public:
        ServerHandler(Server& s) : server{s} {}
        void handle(JoinMessage& msg, User &from);
        void handle(MessageMessage &msg, User &from);
        void handle(QuitMessage &msg, User &from);
        void handle(Message& msg, User &from);
    private:
        Server& server;
    };
    ServerHandler handler;

    void disconnect_user(User &from);
    bool handle_poll_event(struct pollfd& item);
    bool accept_connections();
};

#endif
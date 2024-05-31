#ifndef SERVER_H
#define SERVER_H

#include "house.h"
#include "message.h"
#include "handler.h"
#include "user.h"
#include "serverSocket.h"

#include <variant>
#include <cstdint>
#include <vector>
#include <utility>
#include <poll.h>
#include <unordered_map>

class Server{
public:
    ~Server();
    Server(Server &other) = delete;
    Server(Server &&other) = default;

    Server& operator=(const Server &other) = delete;
    Server& operator=(Server &&other) = default;

    static std::variant<Server, int> create(uint16_t port);

    void run();

private:
    Server(ServerSocket sock) : sock(std::move(sock)), poll_items{}, house{}, handler(*this) {}

    ServerSocket sock;
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
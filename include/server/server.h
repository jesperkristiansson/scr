#ifndef SERVER_H
#define SERVER_H

#include "server/house.h"
#include "server/user.h"
#include "server/serverSocket.h"
#include "server/serverHandler.h"
#include "server/userDatabase.h"
#include "server/logDatabase.h"

#include "common/clientMessage.h"

#include <variant>
#include <cstdint>
#include <vector>
#include <utility>
#include <poll.h>
#include <unordered_map>

class Server{
    friend class ServerHandler;
public:
    ~Server();
    Server(Server &other) = delete;
    Server(Server &&other);

    Server& operator=(const Server &other) = delete;
    Server& operator=(Server &&other);

    static std::variant<Server, int> create(uint16_t port, const std::string &user_db, const std::string &log_dir);

    void run();

private:
    Server(ServerSocket &&sock, const std::string &user_db, const std::string &log_dir) : sock(std::move(sock)), poll_items{}, house{}, user_db(user_db), log_db(log_dir), handler(this) {}

    ServerSocket sock;
    std::vector<struct pollfd> poll_items;
    House house;
    std::unordered_map<int, User> users;
    UserDatabase user_db;
    LogDatabase log_db;

    ServerHandler handler;

    void disconnect_user(User &from);
    bool handle_poll_event(struct pollfd& item);
    bool accept_connections();
};

#endif
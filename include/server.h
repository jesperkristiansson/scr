#ifndef SERVER_H
#define SERVER_H

#include "house.h"

#include <cstdint>
#include <vector>
#include <poll.h>

class Server{
public:
    Server(int server_fd) : server_fd{server_fd}, poll_items{}, house{} {}
    ~Server();
    void run();

private:
    int server_fd;
    std::vector<struct pollfd> poll_items;
    House house;

    bool handle_poll_event(struct pollfd& item);
    bool accept_connections();
};

#endif
#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <variant>
#include <cstdint>

class ServerSocket{
    public:
        ~ServerSocket();
        ServerSocket(ServerSocket &other) = delete;
        ServerSocket(ServerSocket &&other);

        ServerSocket& operator=(const ServerSocket &other) = delete;
        ServerSocket& operator=(ServerSocket &&other);

        static std::variant<ServerSocket, int> create(uint16_t port);
        int accept_connection();
        bool make_nonblocking();
    private:
        ServerSocket(int sock_fd) : sock_fd{sock_fd} {}
        int sock_fd;
};


#endif
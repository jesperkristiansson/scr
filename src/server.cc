#include "utils.h"

#include <iostream>
#include <string>
#include <vector>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/poll.h>
#include <asm-generic/socket.h>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

int server_setup(uint16_t port){
    std::cout << "Starting server at port " << port << std::endl;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
        perror("socket()");
        return -1;
    }

    const int enable = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1){
        perror("setsockopt(SO_REUSEADDR)");
        return -1;
    }

    struct sockaddr_in server_sa;
    memset(&server_sa, 0, sizeof(server_sa));
    server_sa.sin_family = AF_INET;
    server_sa.sin_port = htons(port);
    server_sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(server_fd, (struct sockaddr *)&server_sa, sizeof(server_sa)) == -1){
        perror("bind()");
        return -1;
    }

    if(listen(server_fd, 32) == -1){
        perror("listen()");
        return -1;
    }

    return server_fd;
}

bool make_nonblocking(int fd){
    int flags = fcntl(fd, F_GETFL);
    if(flags == -1){
        perror("fcntl(F_GETFL)");
        return false;
    }
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags) == 0;
}

//assumes non-blocking socket
bool accept_connections(int server_fd, std::vector<struct pollfd> &vec){
    while(1){
        struct sockaddr_in client_sa;
        socklen_t client_sa_len;
        client_sa_len = sizeof(client_sa);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_sa, &client_sa_len);
        if(client_fd == -1){
            //no more incoming connections
            if(errno == EWOULDBLOCK){
                break;
            }
            perror("accept()");
            return false;
        }
        const char *client_ip = inet_ntoa(client_sa.sin_addr);
        uint16_t client_port = ntohs(client_sa.sin_port);
        std::cout << "Client connected: " << client_ip << ":" << client_port << std::endl;
        struct pollfd client = {.fd = client_fd, .events = POLLIN, .revents = 0};
        vec.push_back(client);
    }

    return true;
}

bool server_loop(int server_fd){
    if(!make_nonblocking(server_fd)){
        return false;
    }
    std::vector<struct pollfd> items;
    constexpr int timeout_ms = 100;
    while(1){
        accept_connections(server_fd, items);
        int num_events = poll(items.data(), items.size(), timeout_ms);
        if(num_events == -1){
            perror("poll()");
            return false;
        } else if(num_events > 0){
            for(auto &item : items){
                if(item.revents == 0){
                    continue;
                }

                //handle events
            }
        }

    }
    return 1;
}

int main(int argc, char **argv){
    assert(argc == 2);
    int port_int;
    try
    {
        port_int = std::stoi(argv[1]);
    }
    catch(const std::exception& e)
    {
        FAIL(e.what());
    }

    uint16_t port;
    if(port_int < 0 || port_int > UINT16_MAX){
        FAIL("Port not in range (0-65535)");
    }
    port = (uint16_t)port_int;

    int server_fd = server_setup(port);
    if(server_fd == -1){
        return EXIT_FAILURE;
    }

    server_loop(server_fd);

    close(server_fd);

    return EXIT_SUCCESS;
}
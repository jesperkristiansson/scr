#include "networking.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

#include <iostream>

int create_listening_socket(uint16_t port){
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1){
        perror("socket()");
        return -1;
    }

    const int enable = 1;
    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1){
        perror("setsockopt(SO_REUSEADDR)");
        return -1;
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock_fd, (struct sockaddr *)&sa, sizeof(sa)) == -1){
        perror("bind()");
        return -1;
    }

    if(listen(sock_fd, 32) == -1){
        perror("listen()");
        return -1;
    }

    return sock_fd;
}

int accept_connection(int sock_fd){
    struct sockaddr_in sa;
    socklen_t sa_len = sizeof(sa);
    int fd = accept(sock_fd, (struct sockaddr *)&sa, &sa_len);
    if(fd == -1){
        //no more incoming connections
        if(errno != EWOULDBLOCK){
            perror("accept()");
        }
        return -1;
    }
    const char *ip = inet_ntoa(sa.sin_addr);
    uint16_t port = ntohs(sa.sin_port);
    std::cout << "Accepted connection on " << ip << ":" << port << std::endl;
    return fd;
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
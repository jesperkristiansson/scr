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

bool make_timeout(int sock_fd, int ms){
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    if(setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))){
        perror("setsockopt(TIMEOUT)");
        return false;
    }
    return true;
}

bool send_data(int sock_fd, const void *data, size_t size){
    while(size){
        std::cout << "sending data" << std::endl;
        int bytes_sent = send(sock_fd, data, size, 0);
        if(bytes_sent == -1){
            perror("send()");
            return false;
        } else if(bytes_sent == 0){
            std::cerr << "send wasn't able to send any data" << std::endl;
            return false;   //?
        } else{
            size -= bytes_sent;
        }
    }
    return true;
}

//assumes timeout is set on socket, could be changed to use poll instead
bool receive_size(int sock_fd, void *buf, size_t size){
    unsigned char *recv_buf = static_cast<unsigned char *>(buf);
    while(size){
        ssize_t bytes_received = recv(sock_fd, recv_buf, size, 0);
        if(bytes_received == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                std::cerr << "recv() timed out" << std::endl;
            }
            perror("receive_size: recv");
            return false;
        } else if(bytes_received == 0){
            //stream closed
            return false;
        } else{
            size -= static_cast<size_t>(bytes_received);
            recv_buf += bytes_received;
        }
    }
    return true;
}

//assumes non-blocking/timeout socket
std::vector<unsigned char> receive_data(int sock_fd){
    constexpr size_t buf_size = 1024;
    unsigned char buf[buf_size];

    std::vector<unsigned char> result;

    ssize_t bytes_received;
    do{
        bytes_received = recv(sock_fd, buf, buf_size, 0);
        if(bytes_received == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                break;
            }
            perror("recv");
            return result;
        } else if(bytes_received == 0){
            //stream closed
        } else{
            result.insert(result.end(), buf, buf + bytes_received);
        }
    } while(bytes_received);
    return result;
}
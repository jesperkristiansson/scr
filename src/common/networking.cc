#include "networking.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

#include <iostream>

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
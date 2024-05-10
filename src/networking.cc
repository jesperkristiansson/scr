#include "networking.h"

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

//assumes non-blocking socket
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

//assumes non-blocking socket
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
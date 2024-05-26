#include "connection.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

#include <iostream>
#include <utility>

Connection::Connection(int sock_fd) : sock_fd{sock_fd} {}

Connection::Connection(Connection &&other) : sock_fd{std::exchange(other.sock_fd, -1)} {}

Connection::~Connection(){
    if(sock_fd == -1){
        return;
    }
    if(close(sock_fd) == -1){
        perror("~Connection: close()");
    }
}

Connection& Connection::operator=(Connection &&other){
    std::swap(this->sock_fd, other.sock_fd);
    return *this;
}

bool Connection::send_data(std::vector<std::byte> data){
    return send_data(data.data(), data.size());
}

bool Connection::send_data(const void *data, size_t size){
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

ssize_t Connection::receive_data(std::byte *buf, size_t size){
    ssize_t bytes_received;
    bytes_received = recv(sock_fd, buf, size, 0);
    if(bytes_received == -1){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            std::cerr << "recv() timed out" << std::endl;
        }
        perror("receive_size: recv");
    }
    return bytes_received;
}
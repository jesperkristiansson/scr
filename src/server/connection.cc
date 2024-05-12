#include "connection.h"
#include "networking.h"

#include <unistd.h>
#include <stdio.h>

Connection::Connection(int sock_fd) : sock_fd{sock_fd} {}

Connection::~Connection(){
    if(close(sock_fd) == -1){
        perror("~Connection: close()");
    }
}

void Connection::send_data(std::vector<unsigned char> data){
    send_data(data.data(), data.size());
}

void Connection::send_data(const void *data, size_t size){
    ::send_data(sock_fd, data, size);
}

std::vector<unsigned char> Connection::receive_data(){
    return ::receive_data(sock_fd);
}

std::vector<unsigned char> Connection::receive_data(size_t size){
    std::vector<unsigned char> result;
    result.reserve(size);
    ::receive_size(sock_fd, result.data(), size);
    return result;
}
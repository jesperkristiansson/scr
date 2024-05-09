#include "utils.h"

#include <cstdio>
#include <iostream>
#include <string>

#include <cstdint>
#include <cstring>
#include <cassert>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

bool send_data(int sock_fd, const void *data, size_t size){
    while(size){
        std::cout << "sending data" << std::endl;
        int bytes_sent = send(sock_fd, data, size, 0);
        if(bytes_sent == -1){
            perror("send()");
            return false;;
        } else if(bytes_sent == 0){
            std::cerr << "send wasn't able to send any data" << std::endl;
            return false;   //?
        } else{
            size -= bytes_sent;
        }
    }
    return true;
}

int main(int argc, char **argv){
    assert(argc == 3);
    //add error checking for IP address
    char *ip = argv[1];
    int port_int;
    try
    {
        port_int = std::stoi(argv[2]);
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

    std::cout << "Client connecting to " << ip << ":" << port << std::endl;

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd == -1){
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_sa;
    memset(&server_sa, 0, sizeof(server_sa));
    server_sa.sin_family = AF_INET;
    server_sa.sin_port = htons(port);
    if(inet_aton(ip, &server_sa.sin_addr) == 0){  //right?
        FAIL("inet_aton()");
    }

    if(connect(client_fd, (struct sockaddr *)&server_sa, sizeof(server_sa)) == -1){
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to: " << inet_ntoa(server_sa.sin_addr) << ":" << ntohs(server_sa.sin_port) << std::endl;
    char msg[200] = "test";
    send_data(client_fd, msg, sizeof(msg));

    close(client_fd);

    return EXIT_SUCCESS;
}
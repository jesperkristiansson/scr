#include "utils.h"

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

    int server_fd = connect(client_fd, (struct sockaddr *)&server_sa, sizeof(server_sa));
    if(server_fd == -1){
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to: " << inet_ntoa(server_sa.sin_addr) << ":" << ntohs(server_sa.sin_port) << std::endl;

    close(server_fd);
    close(client_fd);

    return EXIT_SUCCESS;
}
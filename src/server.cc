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

    std::cout << "Server running at port " << port << std::endl;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    const int enable = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1){
        perror("setsockopt(SO_REUSEADDR)");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_sa;
    memset(&server_sa, 0, sizeof(server_sa));
    server_sa.sin_family = AF_INET;
    server_sa.sin_port = htons(port);
    server_sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(server_fd, (struct sockaddr *)&server_sa, sizeof(server_sa)) == -1){
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 32) == -1){
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_sa;
    socklen_t client_sa_len;
    client_sa_len = sizeof(client_sa);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_sa, &client_sa_len);
    if(client_fd == -1){
        perror("accept()");
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connected: " << inet_ntoa(client_sa.sin_addr) << ":" << ntohs(client_sa.sin_port) << std::endl;


    close(client_fd);
    close(server_fd);

    return EXIT_SUCCESS;
}
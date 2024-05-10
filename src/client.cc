#include "utils.h"
#include "networking.h"
#include "protocol.h"

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
#include <fcntl.h>
#include <poll.h>

inline std::string read_line(){
    std::string line;
    std::getline(std::cin, line);
    return line;
}

bool client_loop(int client_fd){
    if(!make_timeout(client_fd, 1000)){
        return false;
    }

    constexpr int stdin_index = 0;
    constexpr int server_index = 1;
    struct pollfd items[2];
    items[stdin_index] = {.fd = STDIN_FILENO, .events = POLLIN, .revents = 0};
    items[server_index] = {.fd = client_fd, .events = POLLIN, .revents = 0};
    struct pollfd &stdin_item = items[stdin_index];
    struct pollfd &server_item = items[server_index];

    constexpr int timeout_ms = 100;
    while(1){
        int num_events = poll(items, 2, timeout_ms);
        if(num_events == -1){
            perror("poll()");
            return false;
        } else if(num_events > 0){
            if(stdin_item.revents){
                std::string msg = read_line();
                struct header header;
                header.type = MESSAGE;
                send_header(client_fd, header);
                send_message(client_fd, msg);
            }
            if(server_item.revents){
                //get packet from server
                struct header header = receive_header(client_fd);
                if(header.type == QUIT){
                    std::cout << "connection lost" << std::endl;
                    close(client_fd);
                    break;
                } else{
                    assert(header.type == MESSAGE);
                    std::string msg = receive_message(client_fd);
                    std::cout << "msg from server: " << msg << std::endl;
                }

                // std::vector<unsigned char> data = receive_data(client_fd);
                // std::string msg(reinterpret_cast<const char *>(data.data()), data.size());
                // std::cout << "msg from server: " << msg << std::endl;
            }
        }

    }
    return 1;
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

    client_loop(client_fd);

    close(client_fd);

    return EXIT_SUCCESS;
}
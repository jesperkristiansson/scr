#include "client/server.h"
#include "client/stringToMessage.h"

#include "common/utils.h"
#include "common/clientMessages.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <memory>

#include <cstdint>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <poll.h>

inline std::string read_line(){
    std::string line;
    std::getline(std::cin, line);
    return line;
}

bool handle_input(Server &server){
    std::string line = read_line();
    MessagePointer msgPtr = string_to_message(line);
    if(!msgPtr){
        std::cerr << "string_to_message returned nullptr" << std::endl;
    }

    return server.send_message(*msgPtr);
}

int handle_message(const Message *msg){
    if(dynamic_cast<const MessageMessage *>(msg)){
        std::cout << "msg from server: " << dynamic_cast<const MessageMessage *>(msg)->msg << std::endl;
    } else if(dynamic_cast<const QuitMessage *>(msg)){
        std::cout << "connection lost" << std::endl;
        return 1;
    } else{
        std::cerr << "handle_message: unrecognized message" << std::endl;
        return -1;
    }

    return 0;
}

bool client_loop(Server &server){
    constexpr int stdin_index = 0;
    constexpr int server_index = 1;
    struct pollfd items[2];
    items[stdin_index] = {.fd = STDIN_FILENO, .events = POLLIN, .revents = 0};
    items[server_index] = {.fd = server.get_fd(), .events = POLLIN, .revents = 0};
    struct pollfd &stdin_item = items[stdin_index];
    struct pollfd &server_item = items[server_index];

    std::vector<std::byte> buffer;

    constexpr int timeout_ms = 100;
    while(1){
        int num_events = poll(items, 2, timeout_ms);
        if(num_events == -1){
            perror("poll()");
            return false;
        } else if(num_events > 0){
            if(stdin_item.revents){
                handle_input(server);
            }
            if(server_item.revents){
                //get packet from server
                ssize_t received = server.receive();
                if(received < 0){
                    //error
                    std::cerr << "server.receive error" << std::endl;
                    return false;
                } else if(received == 0){
                    //connection closed
                    return true;
                }

                MessagePointer mp;
                MessageErrorStatus status = server.get_message(mp);
                switch(status){
                    case MessageErrorStatus::Success:
                        break;
                    default:
                        return true;
                        break;
                }

                int res = handle_message(mp.get());
                if(res == -1){
                    return false;
                } else if(res == 1){
                    break;
                }
            }
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

    std::cout << "Connecting to " << ip << ":" << port << std::endl;
    Server server = Server::connect(ip, port);
    if(!server.valid()){
        return EXIT_FAILURE;
    }

    client_loop(server);

    return EXIT_SUCCESS;
}
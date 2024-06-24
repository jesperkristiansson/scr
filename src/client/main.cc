#include "client/server.h"
#include "client/stringToMessage.h"

#include "common/utils.h"
#include "common/clientMessages.h"
#include "common/serverMessages.h"

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
#include <termios.h>

static std::string username;

inline std::string read_line(){
    std::string line;
    std::getline(std::cin, line);
    return line;
}

inline std::string read_line_hidden(){
    struct termios old_term, new_term;
    if(tcgetattr(STDIN_FILENO, &old_term) != 0){
        //error;
        return std::string();
    }
    new_term = old_term;
    new_term.c_lflag &= ~ECHO;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term) != 0){
        //error;
        return std::string();
    }
    //read input
    std::string out = read_line();

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term) != 0){
        //error;
        return std::string();
    }

    return out;
}

bool handle_input(Server &server){
    std::string line = read_line();
    ClientMessage::MessagePointer msgPtr = string_to_message(line);
    if(!msgPtr){
        std::cerr << "string_to_message returned nullptr" << std::endl;
    }

    return server.send_message(*msgPtr);
}

int handle_message(const ServerMessage *msg){
    if(dynamic_cast<const ServerMessages::MessageMessage *>(msg)){
        const ServerMessages::MessageMessage *message = dynamic_cast<const ServerMessages::MessageMessage *>(msg);
        std::cout << message->from_name << ": " << message->msg << std::endl;
    } else if(dynamic_cast<const ServerMessages::QuitMessage *>(msg)){
        std::cout << "connection lost" << std::endl;
        return 1;
    } else if(dynamic_cast<const ServerMessages::LoginResultMessage *>(msg)){
        //not implemented
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

                ServerMessage::MessagePointer mp;
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

bool log_in(Server &server){
    std::cout << "Username: " << std::flush;
    username = read_line();
    std::cout << "Password: " << std::flush;
    std::string password = read_line_hidden();

    //send login request
    ClientMessages::LoginMessage message(username, password);
    server.send_message(message);

    //check answer
    ssize_t received = server.receive();
    if(received < 0){
        //error
        std::cerr << "server.receive error" << std::endl;
        return false;
    } else if(received == 0){
        //connection closed
        return true;
    }

    ServerMessage::MessagePointer mp;
    MessageErrorStatus status = server.get_message(mp);
    switch(status){
        case MessageErrorStatus::Success:
            break;
        default:
            return true;
            break;
    }

    auto mesg = dynamic_cast<const ServerMessages::LoginResultMessage *>(mp.get());
    if(!mesg){
        //wrong message type
        return false;
    }

    return mesg->result == 1;
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

    bool success = log_in(server);
    if(!success){
        std::cerr << "Log in failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Logged in as " << username << std::endl;

    client_loop(server);

    return EXIT_SUCCESS;
}
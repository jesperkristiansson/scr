#include "client/client.h"
#include "client/server.h"
#include "client/stringToMessage.h"

#include <poll.h>
#include <unistd.h>

std::variant<Client, int> Client::create(Screen &screen, const char *ip, uint16_t port){
    auto serv = Server::connect(ip, port);
    //error returned
    if(!serv.valid()){
        std::cerr << "error creating Server" << std::endl;
        return serv.get_fd();
    }
    return Client(screen, std::move(serv));
}

bool Client::log_in(){
    std::string password;
    screen.get_login(username, password);

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

bool Client::client_loop(){
    constexpr int stdin_index = 0;
    constexpr int server_index = 1;
    struct pollfd items[2];
    items[stdin_index] = {.fd = STDIN_FILENO, .events = POLLIN, .revents = 0};
    items[server_index] = {.fd = server.get_fd(), .events = POLLIN, .revents = 0};
    struct pollfd &stdin_item = items[stdin_index];
    struct pollfd &server_item = items[server_index];

    screen.start_app();
    screen.set_info("Simple Chat Room");

    constexpr int timeout_ms = 100;
    while(1){
        int num_events = poll(items, 2, timeout_ms);
        if(num_events == -1){
            perror("poll()");
            return false;
        } else if(num_events > 0){
            if(stdin_item.revents){
                handle_input();
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

bool Client::handle_input(){
    std::string line;
    if(!screen.get_input(line)){
        return true;
    }

    ClientMessage::MessagePointer msgPtr = string_to_message(line);
    if(!msgPtr){
        std::cerr << "string_to_message returned nullptr" << std::endl;
    }

    return server.send_message(*msgPtr);
}

int Client::handle_message(const ServerMessage *msg){
    if(dynamic_cast<const ServerMessages::MessageMessage *>(msg)){
        const ServerMessages::MessageMessage *message = dynamic_cast<const ServerMessages::MessageMessage *>(msg);
        std::string line = message->from_name + ": " + message->msg;
        screen.put_message(line);
        screen.update_screen();
        //std::cout << message->from_name << ": " << message->msg << std::endl;
    } else if(dynamic_cast<const ServerMessages::QuitMessage *>(msg)){
        screen.put_message("Connection lost");
        screen.update_screen();
        //std::cout << "connection lost" << std::endl;
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
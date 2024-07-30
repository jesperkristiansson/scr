#include "client/client.h"
#include "client/server.h"

#include <ctime>
#include <iomanip>
#include <sstream>
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

Client::Client(Client &&other) :
    server(std::move(other.server)),
    screen{other.screen},
    username(std::move(other.username)),
    handler(this)
{}

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

void Client::put_error(const std::string &msg){
    std::string out = "<Error>: " + msg;
    screen.put_message(out);
}

bool Client::handle_input(){
    std::string line;
    if(!screen.get_input(line)){
        return true;
    }

    return handler.handleInput(line);
}

int Client::handle_message(const ServerMessage *msg){
    if(dynamic_cast<const ServerMessages::MessageMessage *>(msg)){
        const ServerMessages::MessageMessage *message = dynamic_cast<const ServerMessages::MessageMessage *>(msg);

        const char *fmt = "%H:%M:%S";
        std::time_t t = static_cast<std::time_t>(message->time);
        std::tm tm = *std::localtime(&t);
        std::ostringstream time_stream;
        time_stream << std::put_time(&tm, fmt);

        std::string line = time_stream.str() + ": " + message->from_name + ": " + message->msg;
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
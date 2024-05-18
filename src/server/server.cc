#include "server.h"
#include "networking.h"
#include "protocol.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

Server::~Server(){
    if(close(server_fd) == -1){
        perror("close(server_fd)");
    }
}

void Server::run(){
    if(!make_nonblocking(server_fd)){
        return;
    }
    constexpr int timeout_ms = 100;
    while(1){
        accept_connections();
        int num_events = poll(poll_items.data(), poll_items.size(), timeout_ms);
        if(num_events == -1){
            perror("poll()");
            return;
        } else if(num_events > 0){
            for(auto &item : poll_items){
                if(!handle_poll_event(item)) return;
            }
        }
    }
}

//assumes non-blocking socket
bool Server::handle_poll_event(struct pollfd& item){
    if(item.revents == 0 || item.events < 0){
        return true;
    }
    //std::cout << item.revents << std::endl;
    // shouldn't ever block, since poll says it has an event
    struct header header = receive_header(item.fd);
    switch(header.type){
        case MESSAGE:
            {
                std::string msg = receive_message(item.fd);
                std::cout << "Received message : \"" << msg << "\" from client " << item.fd << std::endl;
                house.echo_message(item.fd, msg);
            }
            break;
        case QUIT:
            std::cout << "Connection closed by client " << item.fd << std::endl;
            if(close(item.fd) == -1){
                perror("close()");
                return false;
            }
            house.remove_user(item.fd);
            //leaves the item in the vector, this should be fixed
            item.events = -1;
            break;
        case JOIN:
            {
                std::string target_room = receive_message(item.fd);
                std::cout << "Received join request to " << target_room << " by client " << item.fd << std::endl;
                house.move_user(item.fd, target_room);
            }
            break;
        default:
            std::cerr << "received unknown packet type from client " << item.fd << ": " << header.type << std::endl;
            //handle this? could reply that it was an unknown type, kill them, or ignore it
    }
    return true;
}

bool Server::accept_connections(){
    while(1){
        int client_fd = accept_connection(server_fd);
        if(client_fd < 0){
            return false;
        }

        make_timeout(client_fd, 1000);
        struct pollfd client = {.fd = client_fd, .events = POLLIN, .revents = 0};
        poll_items.push_back(client);
        house.add_user(client_fd);
    }

    return true;
}
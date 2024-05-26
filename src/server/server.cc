#include "server.h"
#include "networking.h"
#include "protocol.h"
#include "message.h"
#include "user.h"

#include <iostream>
#include <algorithm>
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

void Server::disconnect_user(User &user){
    std::cout << "Connection closed by client " << user.get_name() << std::endl;
    //close connection
    int client_fd = user.get_fd();

    house.remove_user(&user);

    auto pollfd_match = [&](const pollfd &item){return item.fd == client_fd;};
    auto it = std::find_if(poll_items.begin(), poll_items.end(), pollfd_match);
    if(it == poll_items.end()){
        std::cerr << "find_if(fd) failed" << std::endl;
    } else{
        poll_items.erase(it);
    }

    users.erase(client_fd);
}

//assumes non-blocking socket
bool Server::handle_poll_event(struct pollfd& item){
    if(item.revents == 0 || item.events < 0){
        return true;
    }
    //std::cout << item.revents << std::endl;
    // shouldn't ever block, since poll says it has an event
    User &user = users.at(item.fd);
    ssize_t received = user.receive();
    if(received < 0){
        //handle error?
        return false;
    } else if(received == 0){
        disconnect_user(user);
        return true;
    }

    MessagePointer mp;
    MessageErrorStatus status = user.get_message(mp);
    switch(status){
        case MessageErrorStatus::Success:
            break;
        default:
            return true;
            break;
    }
    mp->dispatch(handler, user);
    return true;
}

bool Server::accept_connections(){
    while(1){
        int client_fd = accept_connection(server_fd);
        if(client_fd < 0){
            return false;
        }

        struct pollfd client = {.fd = client_fd, .events = POLLIN, .revents = 0};
        poll_items.push_back(client);

        std::string name = std::string("User") + std::to_string(client_fd);
        auto pair = users.emplace(client_fd, User(name, client_fd));
        if(!pair.second){
            std::cerr << "users.emplace failed" << std::endl;
            return false;
        }
        User *user = &pair.first->second;
        house.add_user(user);
    }

    return true;
}
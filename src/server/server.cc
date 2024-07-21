#include "server/server.h"
#include "server/user.h"

#include "common/clientMessage.h"
#include "common/serverMessages.h"

#include <iostream>
#include <algorithm>
#include <utility>
#include <stdio.h>
#include <unistd.h>

std::variant<Server, int> Server::create(uint16_t port, const std::string &user_db, const std::string &log_dir){
    auto res = ServerSocket::create(port);
    //error returned
    if(res.index() != 0){
        std::cerr << "error creating ServerSocket" << std::endl;
        return std::get<1>(res);
    }
    return Server(std::move(std::get<0>(res)), user_db, log_dir);
}


Server::Server(Server &&other) :
    sock(std::move(other.sock)),
    poll_items(std::move(other.poll_items)),
    house(std::move(other.house)),
    user_db(std::move(other.user_db)),
    log_db(std::move(other.log_db)),
    handler(this)
    {}

Server& Server::operator=(Server &&other){
    this->sock = std::move(other.sock);
    this->poll_items = std::move(other.poll_items);
    this->house = std::move(other.house);
    return *this;
}

Server::~Server(){}

void Server::run(){
    if(!sock.make_nonblocking()){
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

    if(user.is_logged_in()){
        house.remove_user(&user);
    }

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

    ClientMessage::MessagePointer mp;
    MessageErrorStatus status = user.get_message(mp);
    switch(status){
        case MessageErrorStatus::Success:
            break;
        default:
            return true;
            break;
    }

    //is user attempting to use privileged message while not logged in?
    if(mp->isPrivileged() && !user.is_logged_in()){
        ServerMessages::NotPrivilegedMessage message;
        user.send_message(message);
        return true;
    }

    mp->dispatch(handler, user);
    return true;
}

bool Server::accept_connections(){
    while(1){
        int client_fd = sock.accept_connection();
        if(client_fd < 0){
            return false;
        }

        struct pollfd client = {.fd = client_fd, .events = POLLIN, .revents = 0};
        poll_items.push_back(client);

        auto pair = users.emplace(client_fd, User(client_fd));
        if(!pair.second){
            std::cerr << "users.emplace failed" << std::endl;
            return false;
        }
    }

    return true;
}
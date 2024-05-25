#include "server.h"
#include "handler.h"
#include "message.h"
#include "messages.h"

#include <iostream>
#include <unistd.h>

void Server::ServerHandler::handle(JoinMessage& msg, int from){
    std::cout << "Received join request to " << msg.room_name << " by client " << from << std::endl;
    server.house.move_user(from, msg.room_name);
}

void Server::ServerHandler::handle(QuitMessage& msg [[maybe_unused]], int from){
    std::cout << "Connection closed by client " << from << std::endl;
    if(close(from) == -1){
        perror("close()");
        return;
    }
    server.house.remove_user(from);
    //leaves the item in the vector, this should be fixed
    //item.events = -1;
}

void Server::ServerHandler::handle(MessageMessage &msg, int from){
    std::cout << "Received message : \"" << msg.msg << "\" from client " << from << std::endl;
    server.house.echo_message(from, msg.msg);
}

void Server::ServerHandler::handle(Message& msg [[maybe_unused]], int from [[maybe_unused]]){
    std::cerr << "Unrecognized message" << std::endl;
}
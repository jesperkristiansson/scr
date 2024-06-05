#include "server/server.h"

#include "common/handler.h"
#include "common/message.h"
#include "common/messages.h"

#include <iostream>
#include <unistd.h>

void Server::ServerHandler::handle(JoinMessage& msg, User &from){
    std::cout << "Received join request to " << msg.room_name << " by client " << from.get_name() << std::endl;
    server->house.move_user(&from, msg.room_name);
}

void Server::ServerHandler::handle(QuitMessage& msg [[maybe_unused]], User &from){
    server->disconnect_user(from);
}

void Server::ServerHandler::handle(MessageMessage &msg, User &from){
    std::cout << "Received message : \"" << msg.msg << "\" from client " << from.get_name() << std::endl;
    server->house.echo_message(&from, msg.msg);
}

void Server::ServerHandler::handle(Message& msg [[maybe_unused]], User &from [[maybe_unused]]){
    std::cerr << "Unrecognized message" << std::endl;
}
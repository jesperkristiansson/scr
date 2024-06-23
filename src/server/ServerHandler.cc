#include "server/serverHandler.h"
#include "server/server.h"

#include "common/clientMessage.h"
#include "common/clientMessages.h"

#include <iostream>
#include <unistd.h>

void ServerHandler::handle(ClientMessages::JoinMessage& msg, User &from){
    std::cout << "Received join request to " << msg.room_name << " by client " << from.get_name() << std::endl;
    server->house.move_user(&from, msg.room_name);
}

void ServerHandler::handle(ClientMessages::QuitMessage& msg [[maybe_unused]], User &from){
    server->disconnect_user(from);
}

void ServerHandler::handle(ClientMessages::MessageMessage &msg, User &from){
    std::cout << "Received message : \"" << msg.msg << "\" from client " << from.get_name() << std::endl;
    server->house.echo_message(&from, msg.msg);
}

void ServerHandler::handle(ClientMessage& msg [[maybe_unused]], User &from [[maybe_unused]]){
    std::cerr << "Unrecognized message" << std::endl;
}
#include "server/serverHandler.h"
#include "server/server.h"

#include "common/clientMessage.h"
#include "common/clientMessages.h"
#include "common/serverMessages.h"

#include <iostream>
#include <ctime>
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

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    struct message_info mi = {.msg = msg.msg, .tm = tm};

    server->log_db.add_message(mi, from.get_room()->getName());
    server->house.echo_message(&from, msg.msg);
}

void ServerHandler::handle(ClientMessages::LoginMessage &msg, User &from){
    std::cout << "Received login-request (" << msg.name << ", " << msg.passwd << ")" << std::endl;
    if(from.is_logged_in()){
        //user is already logged in, reject?
    }

    bool success = server->user_db.user_exists(msg.name, msg.passwd);

    from.log_in(msg.name);
    server->house.add_user(&from);
    //reply with login_success
    ServerMessages::LoginResultMessage res_msg(static_cast<uint16_t>(success));
    from.send_message(res_msg);
}

void ServerHandler::handle(ClientMessage& msg [[maybe_unused]], User &from [[maybe_unused]]){
    std::cerr << "Unrecognized message" << std::endl;
}
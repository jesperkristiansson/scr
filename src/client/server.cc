#include "client/server.h"

ssize_t Server::receive(std::size_t bytes){
    return msgConn.receive(bytes);
}

MessageErrorStatus Server::get_message(ServerMessage::MessagePointer &mp){
    return msgConn.get_message(mp);
}

bool Server::send_message(const ClientMessage &message){
    return msgConn.send_message(message);
}
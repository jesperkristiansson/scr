#include "server/user.h"

User::User(int sock_fd) : room{nullptr}, msgConn(sock_fd) {}

void User::set_room(Room *new_room){
    room = new_room;
}

Room *User::get_room() const{
    return room;
}

void User::log_in(const std::string &name){
    logged_in = true;
    this->username = name;
}

ssize_t User::receive(std::size_t bytes){
    return msgConn.receive(bytes);
}

MessageErrorStatus User::get_message(ClientMessage::MessagePointer &mp){
    return msgConn.get_message(mp);
}

bool User::send_message(const ServerMessage &message){
    return msgConn.send_message(message);
}
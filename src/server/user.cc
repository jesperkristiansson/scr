#include "server/user.h"

User::User(std::string name, int sock_fd) : room{nullptr}, username{name}, msgConn(sock_fd) {}

void User::set_room(Room *new_room){
    room = new_room;
}

Room *User::get_room() const{
    return room;
}

ssize_t User::receive(std::size_t bytes){
    return msgConn.receive(bytes);
}

MessageErrorStatus User::get_message(Message::MessagePointer &mp){
    return msgConn.get_message(mp);
}

bool User::send_message(const Message &message){
    return msgConn.send_message(message);
}
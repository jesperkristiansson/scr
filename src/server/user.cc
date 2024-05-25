#include "user.h"
#include "messages.h"

#include <iostream>

User::User(std::string name, std::string room, int sock_fd) : room{room}, username{name}, connection(sock_fd) {}

void User::set_room(std::string new_room){
    room = new_room;
}

bool User::receive(std::size_t bytes){
    std::size_t pre_size = buffer.size();
    buffer.resize(pre_size + bytes);
    std::byte *recv_buf = &buffer[pre_size];
    if(!connection.receive_data(recv_buf, bytes)){
        //error or closed
        std::cerr << "receive_data returned false" << std::endl;
        return false;
    }

    buffer.resize(pre_size + bytes);
    return true;
}

MessageErrorStatus User::get_message(MessagePointer &mp){
    MessageErrorStatus status;

    MessageType type;
    const std::byte *data = buffer.data();
    std::size_t size = buffer.size()*sizeof(buffer[0]);
    status = peek(data, size, type);
    if(status != MessageErrorStatus::Success){
        return status;
    }

    mp = CreateMessage(type);
    if(mp == nullptr){
        return MessageErrorStatus::Other;
    }

    status = mp->read(data, size);
    if(status != MessageErrorStatus::Success){
        return status;
    }

    return MessageErrorStatus::Success;
}

bool User::send_message(const Message &message){
    bool ret = true;

    std::size_t size = message.size();
    std::byte *buf = new std::byte[size];

    std::byte *buf_cpy = buf;
    std::size_t size_cpy = size;
    MessageErrorStatus status = message.write(buf_cpy, size_cpy);
    if(status == MessageErrorStatus::Success){
        connection.send_data(buf, size);
        ret = false;
    }

    delete buf;

    return ret;
}
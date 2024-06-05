#include "common/messageConnection.h"
#include "common/clientMessages.h"

#include <iostream>

ssize_t MessageConnection::receive(std::size_t bytes){
    std::size_t pre_size = buffer.size();
    buffer.resize(pre_size + bytes);
    std::byte *recv_buf = &buffer[pre_size];
    ssize_t received = connection.receive_data(recv_buf, bytes);
    if(received < 0){
        //error or closed
        std::cerr << "receive_data error" << std::endl;
        return received;
    }

    buffer.resize(pre_size + received);
    return received;
}

MessageErrorStatus MessageConnection::get_message(MessagePointer &mp){
    MessageErrorStatus status;

    MessageType type;
    const std::byte *data = buffer.data();
    std::size_t size = buffer.size()*sizeof(buffer[0]);
    std::size_t size_cpy = size;
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

    //shift buffer
    buffer.erase(buffer.begin(), buffer.begin() + size_cpy);

    return MessageErrorStatus::Success;
}

bool MessageConnection::send_message(const Message &message){
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

    delete[] buf;

    return ret;
}
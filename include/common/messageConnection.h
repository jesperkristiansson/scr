#ifndef MESSAGE_CONNECTION_H
#define MESSAGE_CONNECTION_H

#include "common/connection.h"
#include "common/clientMessage.h"
#include "common/clientMessages.h"

#include <memory>
#include <vector>
#include <utility>
#include <iostream>

template<typename TSendMessage, typename TRecvMessage>
class MessageConnection{
    public:
        MessageConnection(int sock_fd) : connection(sock_fd) {}
        static MessageConnection connect(const char *ip, uint16_t port) {return MessageConnection(Connection::connect(ip, port));}
        bool valid() const {return connection.valid();}
        int get_fd() const {return connection.get_fd();}
        ssize_t receive(std::size_t bytes = 1024);
        MessageErrorStatus get_message(typename TRecvMessage::MessagePointer &mp);
        bool send_message(const TSendMessage& message);
    private:
        MessageConnection(Connection &&conn) : connection(std::move(conn)) {}
        Connection connection;
        std::vector<std::byte> buffer;
};

template<typename TSendMessage, typename TRecvMessage>
ssize_t MessageConnection<TSendMessage, TRecvMessage>::receive(std::size_t bytes){
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

template<typename TSendMessage, typename TRecvMessage>
MessageErrorStatus MessageConnection<TSendMessage, TRecvMessage>::get_message(typename TRecvMessage::MessagePointer &mp){
    MessageErrorStatus status;

    typename TRecvMessage::MessageType type;
    const std::byte *data = buffer.data();
    std::size_t size = buffer.size()*sizeof(buffer[0]);
    std::size_t size_cpy = size;
    status = TRecvMessage::peek(data, size, type);
    if(status != MessageErrorStatus::Success){
        return status;
    }

    mp = TRecvMessage::CreateMessage(type);
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

template<typename TSendMessage, typename TRecvMessage>
bool MessageConnection<TSendMessage, TRecvMessage>::send_message(const TSendMessage &message){
    bool ret = false;

    std::size_t size = message.size();
    std::byte *buf = new std::byte[size];

    std::byte *buf_cpy = buf;
    std::size_t size_cpy = size;
    MessageErrorStatus status = message.write(buf_cpy, size_cpy);
    if(status == MessageErrorStatus::Success){
        connection.send_data(buf, size);
        ret = true;
    }

    delete[] buf;

    return ret;
}

#endif
#ifndef SERVER_H
#define SERVER_H

#include "common/clientMessage.h"
#include "common/messageConnection.h"

#include <memory>
#include <utility>

class Server{
    public:
        Server(int sock_fd) : msgConn(sock_fd) {}
        static Server connect(const char *ip, uint16_t port) {return Server(ClientMessageConnection::connect(ip, port));}
        bool valid() const {return msgConn.valid();}
        int get_fd() const {return msgConn.get_fd();}
        ssize_t receive(std::size_t bytes = 1024);
        MessageErrorStatus get_message(Message::MessagePointer &mp);
        bool send_message(const Message& message);
    private:
        using ClientMessageConnection = MessageConnection<Message, Message>;
        Server(ClientMessageConnection &&msgConn) : msgConn(std::move(msgConn)) {}
        ClientMessageConnection msgConn;
};

#endif
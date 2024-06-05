#ifndef MESSAGE_CONNECTION_H
#define MESSAGE_CONNECTION_H

#include "common/connection.h"
#include "common/clientMessage.h"

#include <memory>
#include <vector>
#include <utility>

class MessageConnection{
    public:
        MessageConnection(int sock_fd) : connection(sock_fd) {}
        static MessageConnection connect(const char *ip, uint16_t port) {return MessageConnection(Connection::connect(ip, port));}
        bool valid() const {return connection.valid();}
        int get_fd() const {return connection.get_fd();}
        ssize_t receive(std::size_t bytes = 1024);
        MessageErrorStatus get_message(MessagePointer &mp);
        bool send_message(const Message& message);
    private:
        MessageConnection(Connection &&conn) : connection(std::move(conn)) {}
        Connection connection;
        std::vector<std::byte> buffer;
};

#endif
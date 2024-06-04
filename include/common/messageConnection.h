#ifndef MESSAGE_CONNECTION_H
#define MESSAGE_CONNECTION_H

#include "common/connection.h"
#include "common/message.h"

#include <memory>
#include <vector>

class MessageConnection{
    public:
        MessageConnection(int sock_fd) : connection(sock_fd)  {}
        int get_fd() const {return connection.get_fd();}
        ssize_t receive(std::size_t bytes = 1024);
        MessageErrorStatus get_message(MessagePointer &mp);
        bool send_message(const Message& message);
    private:
        Connection connection;
        std::vector<std::byte> buffer;
};

#endif
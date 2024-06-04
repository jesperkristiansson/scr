#ifndef SERVER_H
#define SERVER_H

#include "common/message.h"
#include "common/messageConnection.h"

#include <memory>

class Server{
    public:
        Server(int sock_fd);
        int get_fd() const {return msgConn.get_fd();}
        ssize_t receive(std::size_t bytes = 1024);
        MessageErrorStatus get_message(MessagePointer &mp);
        bool send_message(const Message& message);
    private:
        MessageConnection msgConn;
};

#endif
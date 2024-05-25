#ifndef USER_H
#define USER_H

#include "connection.h"
#include "messages.h"

#include <string>
#include <vector>
#include <memory>

class User{
    public:
        User(std::string name, std::string room, int sock_fd);
        void set_room(std::string new_room);
        bool receive(std::size_t bytes = 1024);
        MessageErrorStatus get_message(MessagePointer &mp);
        bool send_message(const Message& message);
    private:
        std::string room;
        std::string username;
        Connection connection;
        std::vector<std::byte> buffer;
};

#endif
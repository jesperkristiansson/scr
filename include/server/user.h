#ifndef USER_H
#define USER_H

#include "common/clientMessage.h"
#include "common/messageConnection.h"

#include <string>
#include <memory>

class Room;

class User{
    public:
        User(std::string name, int sock_fd);
        void set_room(Room *new_room);
        Room *get_room() const;
        std::string get_name() const {return username;}
        int get_fd() const {return msgConn.get_fd();}
        ssize_t receive(std::size_t bytes = 1024);
        MessageErrorStatus get_message(Message::MessagePointer &mp);
        bool send_message(const Message& message);
    private:
        using ServerMessageConnection = MessageConnection<Message, Message>;
        Room *room;
        std::string username;
        ServerMessageConnection msgConn;
};

#endif
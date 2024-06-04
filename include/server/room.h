#ifndef ROOM_H
#define ROOM_H

#include "server/user.h"

#include <unordered_set>
#include <string>

class Room{
    public:
        Room(std::string name) : name{name} {}
        std::string getName() const {return name;}
        bool add_member(User *member);
        bool remove_member(User *member);
        bool send_message_from(std::string message, User *from);
    private:
        std::string name;
        std::unordered_set<User *> members;
};

#endif
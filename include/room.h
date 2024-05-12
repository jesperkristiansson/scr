#ifndef ROOM_H
#define ROOM_H

#include <unordered_set>
#include <string>

class Room{
    public:
        Room(std::string name) : name{name} {}
        std::string getName() const {return name;}
        bool add_member(int member);
        bool remove_member(int member);
        bool send_message_from(std::string message, int from);
    private:
        std::string name;
        std::unordered_set<int> members;
};

#endif
#ifndef ROOM_H
#define ROOM_H

#include <unordered_set>
#include <string>

class room{
    public:
        room(std::string name) : name{name} {}
        bool add_member(int member);
        bool remove_member(int member);
        bool send_message_from(std::string message, int from);
    private:
        std::string name;
        std::unordered_set<int> members;
};

#endif
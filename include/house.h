#ifndef HOUSE_H
#define HOUSE_H

#include "room.h"

#include <unordered_map>

static constexpr const char * default_room_name = "default";

class house{
    public:
        house() : rooms{{default_room_name, room(default_room_name)}} {}
        bool add_user(int user, const std::string &room_name = default_room_name);
        bool remove_user(int user);
        bool move_user(int user, const std::string &room_name);
        bool echo_message(int user, const std::string &message);
    private:
        std::unordered_map<std::string, room> rooms;
        std::unordered_map<int, std::string> users;    //could be replaced with a user-class, keeping track of its room

        void add_user_to_room(int user, const std::string& room_name);
        bool remove_user_from_room(int user, const std::string& room_name);
};

#endif
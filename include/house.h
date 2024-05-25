#ifndef HOUSE_H
#define HOUSE_H

#include "room.h"
#include "user.h"

#include <unordered_map>

static constexpr const char * default_room_name = "default";

class House{
    public:
        House() : rooms{{default_room_name, Room(default_room_name)}} {}

        bool add_user(User *user, const std::string &room_name = default_room_name);
        bool remove_user(User *user);
        bool move_user(User * user, const std::string &room_name);
        bool echo_message(User * user, const std::string &message);
    private:
        std::unordered_map<std::string, Room> rooms;
};

#endif
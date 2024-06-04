#include "server/house.h"

#include <iostream>

bool House::add_user(User *user, const std::string &room_name){
    auto room_pair = rooms.emplace(room_name, Room(room_name));
    Room &room = room_pair.first->second;
    return room.add_member(user);
}

bool House::remove_user(User *user){
    return user->get_room()->remove_member(user);
}

bool House::move_user(User *user, const std::string &room_name){
    if(!remove_user(user) || !add_user(user, room_name)){
        return false;
    }

    return true;
}

bool House::echo_message(User *user, const std::string &message){
    return user->get_room()->send_message_from(message, user);
}
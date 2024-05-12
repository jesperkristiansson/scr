#include "house.h"

#include <iostream>

void house::add_user_to_room(int user, const std::string& room_name){
    auto room_pair = rooms.emplace(room_name, room(room_name));
    room &room = room_pair.first->second;
    room.add_member(user);
}

bool house::remove_user_from_room(int user, const std::string& room_name){
    auto it = rooms.find(room_name);
    if(it == rooms.end()){
        std::cerr << "house::remove_user_from_room: room " << room_name << " doesn't exist" << std::endl;
        return false;
    }
    room &room = it->second;
    return room.remove_member(user);
}

bool house::add_user(int user, const std::string &room_name){
    auto user_pair = users.emplace(user, room_name);
    if(!user_pair.second){
        std::cerr << "house::add_user: user " << user << " already exists" << std::endl;
        return false;
    }
    add_user_to_room(user, room_name);
    return true;
}

bool house::remove_user(int user){
    auto it = users.find(user);
    if(it == users.end()){
        std::cerr << "house::remove_user: user " << user << " doesn't exist" << std::endl;
        return false;
    }
    const std::string &room_name = it->second;
    remove_user_from_room(user, room_name);
    users.erase(it);
    return true;
}

bool house::move_user(int user, const std::string &room_name){
    auto it = users.find(user);
    if(it == users.end()){
        std::cerr << "house::move_user: user " << user << " doesn't exist" << std::endl;
        return false;
    }
    std::string &current_room_name = it->second;
    remove_user_from_room(user, current_room_name);
    add_user_to_room(user, room_name);
    current_room_name = room_name;

    return true;
}

bool house::echo_message(int user, const std::string &message){
    return rooms.at(users.at(user)).send_message_from(message, user);
}
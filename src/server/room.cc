#include "server/room.h"

#include "common/clientMessages.h"

bool Room::add_member(User *member){
    if(members.insert(member).second){
        member->set_room(this);
        return true;
    } else{
        return false;
    }
}

bool Room::remove_member(User *member){
    if(members.erase(member) == 1){
        member->set_room(nullptr);
        return true;
    } else{
        return false;
    }
}

bool Room::send_message_from(std::string message, User *from){
    MessageMessage msg(message);
    for(User *member : members){
        if(member == from){
            continue;
        }
        if(!member->send_message(msg)){
            return false;
        }
    }
    return true;
}
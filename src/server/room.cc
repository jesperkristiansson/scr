#include "server/room.h"

#include "common/serverMessages.h"

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

bool Room::send_message_from(std::string message, uint32_t time, User *from){
    ServerMessages::MessageMessage msg(time, from->get_name(), message);
    for(User *member : members){
        if(!member->send_message(msg)){
            return false;
        }
    }
    return true;
}
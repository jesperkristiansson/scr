#include "room.h"
#include "protocol.h"

bool Room::add_member(int member){
    return members.insert(member).second;
}

bool Room::remove_member(int member){
    return members.erase(member) == 1;
}

bool Room::send_message_from(std::string message, int from){
    struct header header;
    header.type = MESSAGE;
    for(const auto &member_fd : members){
        if(member_fd == from){
            continue;
        }
        send_header(member_fd, header);
        send_message(member_fd, message);
    }
    return true;
}
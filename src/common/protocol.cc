#include "protocol.h"
#include "networking.h"

#include <iostream>
#include <string>
#include <arpa/inet.h>

bool send_header(int sock_fd, struct header header){
    header.type = static_cast<enum packet_type>(htons(static_cast<uint16_t>(header.type)));
    return send_data(sock_fd, &header, sizeof(header));
}

bool send_message(int sock_fd, const std::string &message){
    uint16_t n_len = htons(message.size());
    std::cout << "sending msg_len" << std::endl;
    if(!send_data(sock_fd, &n_len, sizeof(n_len))){
        return false;
    }
    std::cout << "sending msg" << std::endl;
    if(!send_data(sock_fd, message.data(), message.size())){
        return false;
    }
    return true;
}

struct header receive_header(int sock_fd){
    struct header header;
    header.type = QUIT;
    if(!receive_size(sock_fd, &header, sizeof(header))){
        return header;
    }
    header.type = static_cast<enum packet_type>(ntohs(static_cast<uint16_t>(header.type)));
    return header;
}

std::string receive_message(int sock_fd){
    uint16_t msg_len;
    std::cout << "receiving msg_len" << std::endl;
    if(!receive_size(sock_fd, &msg_len, sizeof(msg_len))){
        return std::string();
    }
    msg_len = ntohs(msg_len);
    std::string msg(msg_len, '\0');
    std::cout << "receiving msg" << std::endl;
    if(!receive_size(sock_fd, msg.data(), msg_len)){
        return std::string();
    }
    return msg;
}
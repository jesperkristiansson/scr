#include "protocol.h"
#include "networking.h"

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <cstring>

size_t header_size() {
    return sizeof(header);
}

size_t message_size(const std::string &msg){
    return sizeof(uint16_t) + msg.size();
}

ssize_t write_header(std::byte *buf, size_t size, const header &header){
    if(size < sizeof(header)){
        return 0;
    }
    *reinterpret_cast<packet_type *>(buf) = header.type;
    return sizeof(header);
}

ssize_t write_message(std::byte *buf, size_t size, const std::string &msg){
    if(size < sizeof(uint16_t) + msg.size()){
        return 0;
    }
    *reinterpret_cast<uint16_t*>(buf) = htons(static_cast<uint16_t>(msg.size()));
    memcpy(buf + sizeof(uint16_t), msg.data(), msg.size());

    return sizeof(uint16_t) + msg.size();
}

ssize_t get_header(const std::byte *buf, size_t size, header &header){
    if(size < sizeof(header)){
        return 0;
    }
    header.type = *reinterpret_cast<const packet_type *>(buf);
    if(header.type >= packet_type::END){
        return -1;
    }
    return sizeof(header);
}

ssize_t get_message(const std::byte *buf, size_t size, std::string &msg){
    if(size < sizeof(uint16_t)){
        return 0;
    }
    uint16_t msg_len = ntohs(*reinterpret_cast<const uint16_t *>(buf));
    buf += sizeof(uint16_t);
    size -= sizeof(uint16_t);
    if(size < msg_len){
        return 0;
    }
    msg = std::string(reinterpret_cast<const char *>(buf), msg_len);
    return sizeof(uint16_t) + msg_len;
}

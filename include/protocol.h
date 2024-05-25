#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include <string>

enum packet_type : uint8_t {
    JOIN,
    MESSAGE,
    QUIT,
};

struct header{
    enum packet_type type;
};

struct message{
    uint16_t len;   //max size of packet is 2^16 - 1 = 65535 bytes.
    char data[];
};

bool send_header(int sock_fd, struct header header);
bool send_message(int sock_fd, const std::string &message);

struct header receive_header(int sock_fd);
std::string receive_message(int sock_fd);

#endif
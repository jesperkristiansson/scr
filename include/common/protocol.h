#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include <string>
#include <memory>

enum packet_type : uint8_t {
    JOIN,
    MESSAGE,
    QUIT,
    END
};

struct header{
    enum packet_type type;
};

struct message{
    uint16_t len;   //max size of packet is 2^16 - 1 = 65535 bytes.
    char data[];    //disallowed in ISO C++, ignore warnings about this until this file can be replaced by the message system
};

size_t header_size();
size_t message_size(const std::string &msg);
ssize_t write_header(std::byte *buf, size_t size, const header &header);
ssize_t write_message(std::byte *buf, size_t size, const std::string &msg);

ssize_t get_header(const std::byte *buf, size_t size, header &header);
ssize_t get_message(const std::byte *buf, size_t size, std::string &msg);

#endif

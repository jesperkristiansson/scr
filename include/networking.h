#ifndef NETWORKING_H
#define NETWORKING_H

#include <vector>
#include <cstddef>
#include <cstdint>

int create_listening_socket(uint16_t port);
int accept_connection(int sock_fd);
bool make_nonblocking(int fd);
bool make_timeout(int sock_fd, int ms);
bool send_data(int sock_fd, const void *data, size_t size);
bool receive_size(int sock_fd, void *buf, size_t size);
std::vector<unsigned char> receive_data(int sock_fd);

#endif
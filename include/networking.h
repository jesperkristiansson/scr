#ifndef NETWORKING_H
#define NETWORKING_H

#include <vector>
#include <cstddef>

bool make_nonblocking(int fd);
bool send_data(int sock_fd, const void *data, size_t size);
std::vector<unsigned char> receive_data(int sock_fd);

#endif
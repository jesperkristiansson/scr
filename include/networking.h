#ifndef NETWORKING_H
#define NETWORKING_H

#include <vector>
#include <cstddef>
#include <cstdint>

int create_listening_socket(uint16_t port);
int accept_connection(int sock_fd);
bool make_nonblocking(int fd);

#endif
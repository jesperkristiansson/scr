#ifndef CONNECTION_H
#define CONNECTION_H

#include <vector>

#include <cstddef>
#include <memory>

class Connection{
    public:
        Connection(int sock_fd);
        ~Connection();
        bool set_timeout(int ms);
        bool send_data(std::vector<std::byte> data);
        bool send_data(const void *data, size_t size);
        std::vector<std::byte> receive_data();
        std::vector<std::byte> receive_data(size_t size);
        bool receive_data(std::byte *buf, size_t &size);
    private:
        int sock_fd;
};


#endif

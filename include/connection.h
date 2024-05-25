#ifndef CONNECTION_H
#define CONNECTION_H

#include <vector>

#include <cstddef>
#include <memory>

class Connection{
    public:
        explicit Connection(int sock_fd);
        Connection(Connection &other) = delete;
        Connection(Connection &&other);
        ~Connection();

        Connection& operator=(const Connection &other) = delete;
        Connection& operator=(Connection &&other);

        int get_fd() const {return sock_fd;}
        bool set_timeout(int ms);
        bool send_data(std::vector<std::byte> data);
        bool send_data(const void *data, size_t size);
        std::vector<std::byte> receive_data();
        std::vector<std::byte> receive_data(size_t size);
        ssize_t receive_data(std::byte *buf, size_t &size);
    private:
        int sock_fd;
};


#endif

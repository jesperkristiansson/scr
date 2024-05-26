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

        static Connection connect(const char *ip, uint16_t port);

        bool valid() const {return sock_fd >= 0;}
        int get_fd() const {return sock_fd;}
        bool send_data(std::vector<std::byte> data);
        bool send_data(const void *data, size_t size);
        ssize_t receive_data(std::byte *buf, size_t size);
    private:
        int sock_fd;
};


#endif

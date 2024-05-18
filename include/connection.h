#ifndef CONNECTION_H
#define CONNECTION_H

#include <vector>

#include <cstddef>

class Connection{
    public:
        Connection(int sock_fd);
        ~Connection();
        bool set_timeout(int ms);
        bool send_data(std::vector<unsigned char> data);
        bool send_data(const void *data, size_t size);
        std::vector<unsigned char> receive_data();
        std::vector<unsigned char> receive_data(size_t size);
        bool receive_data(void *buf, size_t size);
    private:
        int sock_fd;
};


#endif

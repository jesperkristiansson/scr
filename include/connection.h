#ifndef CONNECTION_H
#define CONNECTION_H

#include <vector>

class Connection{
    public:
        Connection(int sock_fd);
        ~Connection();
        void send_data(std::vector<unsigned char> data);
        void send_data(const void *data, size_t size);
        std::vector<unsigned char> receive_data();
        std::vector<unsigned char> receive_data(size_t size);
    private:
        int sock_fd;
};


#endif
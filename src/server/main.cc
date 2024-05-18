#include "utils.h"
#include "server.h"
#include "networking.h"

#include <iostream>

#include <cstdint>
#include <cassert>

int main(int argc, char **argv){
    assert(argc == 2);
    int port_int;
    try
    {
        port_int = std::stoi(argv[1]);
    }
    catch(const std::exception& e)
    {
        FAIL(e.what());
    }

    uint16_t port;
    if(port_int < 0 || port_int > UINT16_MAX){
        FAIL("Port not in range (0-65535)");
    }
    port = (uint16_t)port_int;

    int server_fd = create_listening_socket(port);
    if(server_fd == -1){
        return EXIT_FAILURE;
    }

    Server server(server_fd);
    server.run();

    return EXIT_SUCCESS;
}
#include "server/server.h"
#include "common/utils.h"

#include <iostream>
#include <variant>
#include <utility>

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

    auto res = Server::create(port);
    if(res.index() != 0){
        std::cerr << "error creating Server" << std::endl;
        return EXIT_SUCCESS;
    }

    Server server = std::move(std::get<0>(res));
    server.run();

    return EXIT_SUCCESS;
}
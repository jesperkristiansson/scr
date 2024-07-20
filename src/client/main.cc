#include "client/screen.h"
#include "client/client.h"

#include "common/utils.h"

#include <cstdio>
#include <iostream>

#include <cstdint>
#include <cassert>


int main(int argc, char **argv){
    assert(argc == 3);
    //add error checking for IP address
    char *ip = argv[1];
    int port_int;
    try
    {
        port_int = std::stoi(argv[2]);
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

    Screen screen;
    auto res = Client::create(screen, ip, port);
    if(res.index() != 0){
        std::cerr << "error creating Client" << std::endl;
        return EXIT_SUCCESS;
    }

    Client client = std::move(std::get<0>(res));

    bool success = client.log_in();
    if(!success){
        std::cerr << "Log in failed" << std::endl;
        return EXIT_FAILURE;
    }

    client.client_loop();

    return EXIT_SUCCESS;
}
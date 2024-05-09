#include <iostream>
#include <string>

#include <cassert>
#include <stdlib.h>

int main(int argc, char **argv){
    assert(argc == 3);
    //add error checking for IP address
    std::string ip = argv[1];
    unsigned int port;
    try
    {
        port = std::stoi(argv[2]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Client connecting to " << ip << ":" << port << std::endl;

    return EXIT_SUCCESS;
}
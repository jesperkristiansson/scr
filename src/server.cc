#include <iostream>
#include <string>

#include <cassert>
#include <stdlib.h>

int main(int argc, char **argv){
    assert(argc == 2);
    unsigned int port;
    try
    {
        port = std::stoi(argv[1]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Server running at port " << port << std::endl;

    return EXIT_SUCCESS;
}
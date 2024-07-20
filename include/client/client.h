#ifndef CLIENT_H

#include "client/screen.h"
#include "client/server.h"

#include "common/clientMessages.h"
#include "common/serverMessages.h"

#include <string>
#include <utility>
#include <variant>

class Client{
public:
    static std::variant<Client, int> create(Screen &screen, const char *ip, uint16_t port);

    bool log_in();
    bool client_loop();
private:
    Server server;
    Screen &screen;
    std::string username;

    Client(Screen &screen, Server &&server) : server(std::move(server)), screen{screen}, username{} {}

    bool handle_input();
    int handle_message(const ServerMessage *msg);
};

#endif
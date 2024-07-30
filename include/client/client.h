#ifndef CLIENT_H

#include "client/screen.h"
#include "client/server.h"
#include "client/inputHandler.h"

#include "common/clientMessages.h"
#include "common/serverMessages.h"

#include <string>
#include <utility>
#include <variant>

class Client{
friend class InputHandler;
public:
    static std::variant<Client, int> create(Screen &screen, const char *ip, uint16_t port);

    Client(Client &&other);

    bool log_in();
    bool client_loop();

    void put_error(const std::string &msg);
    void set_room(const std::string &room);
private:
    Server server;
    Screen &screen;
    std::string username;
    InputHandler handler;

    Client(Screen &screen, Server &&server) : server(std::move(server)), screen{screen}, username{}, handler(this) {}

    bool handle_input();
    int handle_message(const ServerMessage *msg);
};

#endif
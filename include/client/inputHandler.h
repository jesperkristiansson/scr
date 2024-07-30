#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <string>
#include <vector>

class Client;

class InputHandler{
public:
    InputHandler(Client *client) : client(client) {}
    void setClient(Client *c){client = c;}
    bool handleInput(const std::string &input);

    bool joinHandler(const std::string &args);
    bool quitHandler(const std::string &args);
    bool helpHandler(const std::string &args);
private:
    Client *client;
};

#endif
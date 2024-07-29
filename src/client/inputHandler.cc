#include "client/inputHandler.h"
#include "client/client.h"
#include "common/clientMessages.h"

#include <iostream>
#include <sstream>
#include <utility>
#include <functional>

namespace{
    using HandlerFunc = bool (InputHandler::*)(const std::string &);

    struct command{
        const char *command_text;
        HandlerFunc handler;
    };

    constexpr static struct command registered_commands[] = {
        {.command_text = "join", .handler = &InputHandler::joinHandler},
        {.command_text = "quit", .handler = &InputHandler::quitHandler},
    };
}

bool InputHandler::handleInput(const std::string &input) {
    if(input.size() && input[0] == '/'){    //is a command
        std::stringstream ss(input);
        std::string command;
        ss >> command;

        for(struct command registered_command : registered_commands){
            if(command.compare(1, std::string::npos, registered_command.command_text) == 0){
                //do command
                return std::invoke(registered_command.handler, this, ss.str());
            }
        }

        std::cerr << "command \"" << command << "\" does not exist" << std::endl;
        return false;
    } else{
        return client->server.send_message(ClientMessages::MessageMessage(input));
    }
}

bool InputHandler::joinHandler(const std::string &input){
    std::string room;
    std::istringstream ss(input);
    ss >> room;
    if(ss.fail()){
        std::cerr << "command /join requires an argument" << std::endl;
        return false;
    }
    return client->server.send_message(ClientMessages::JoinMessage(room));
}

bool InputHandler::quitHandler(const std::string &input [[maybe_unused]]){
    return client->server.send_message(ClientMessages::QuitMessage());
}
#include "client/inputHandler.h"
#include "client/client.h"
#include "common/clientMessages.h"

#include <sstream>
#include <functional>

namespace{
    using HandlerFunc = bool (InputHandler::*)(const std::string &);

    struct command{
        const char *command_text;
        HandlerFunc handler;
        std::string usage;
        std::string description;
    };

    const struct command registered_commands[] = {
        {.command_text = "join", .handler = &InputHandler::joinHandler, .usage="/join <room>", .description="Changes chat room to <room>."},
        {.command_text = "quit", .handler = &InputHandler::quitHandler, .usage="/quit", .description="Quits this application."},
        {.command_text = "help", .handler = &InputHandler::helpHandler, .usage="/help", .description="Displays this help text."},
    };
}

bool InputHandler::handleInput(const std::string &input) {
    if(input.size() && input[0] == '/'){    //is a command
        std::istringstream ss(input);
        std::string command;
        ss >> command;

        for(const struct command &registered_command : registered_commands){
            if(command.compare(1, std::string::npos, registered_command.command_text) == 0){
                //do command
                std::string remainder;
                getline(ss, remainder);
                bool ret = std::invoke(registered_command.handler, this, remainder);
                client->screen.update_screen();
                return ret;
            }
        }

        std::string err = "command \"" + command + "\" does not exist";
        client->put_error(err);
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
        std::string err = "command /join requires an argument";
        client->put_error(err);
        return false;
    }
    client->set_room(room); //todo: set room when receiving ack from server instead
    return client->server.send_message(ClientMessages::JoinMessage(room));
}

bool InputHandler::quitHandler(const std::string &input [[maybe_unused]]){
    return client->server.send_message(ClientMessages::QuitMessage());
}

bool InputHandler::helpHandler(const std::string &input [[maybe_unused]]){
    std::string message = "Help:";
    for(const struct command &command : registered_commands){
        message += '\n' + command.usage + " - " + command.description;
    }
    client->screen.put_message(message);
    return true;
}
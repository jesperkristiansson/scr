#include "client/stringToMessage.h"

#include "common/clientMessages.h"

#include <iostream>
#include <sstream>
#include <utility>

namespace{  //file private
    using MessageCreator = ClientMessage::MessagePointer (*)(std::stringstream);

    ClientMessage::MessagePointer JoinCreator(std::stringstream ss){
        std::string room;
        ss >> room;
        if(ss.fail()){
            std::cerr << "command /join requires an argument" << std::endl;
            return ClientMessage::MessagePointer();
        }
        return ClientMessage::MessagePointer(new ClientMessages::JoinMessage(room));
    }

    ClientMessage::MessagePointer QuitCreator(std::stringstream ss [[maybe_unused]]){
        return ClientMessage::MessagePointer(new ClientMessages::QuitMessage());
    }

    struct command{
        const char *command_text;
        MessageCreator creator;
    };

    constexpr struct command registered_commands[] = {
        {.command_text = "join", .creator = JoinCreator},
        {.command_text = "quit", .creator = QuitCreator},
    };
}

ClientMessage::MessagePointer string_to_message(const std::string &string){
    if(string.size() && string[0] == '/'){    //is a command
        std::stringstream ss(string);
        std::string command;
        ss >> command;

        for(struct command registered_command : registered_commands){
            if(command.compare(1, std::string::npos, registered_command.command_text) == 0){
                //do command
                return registered_command.creator(std::move(ss));
            }
        }

        std::cerr << "command \"" << command << "\" does not exist" << std::endl;
        return ClientMessage::MessagePointer();
    } else{
        return ClientMessage::MessagePointer(new ClientMessages::MessageMessage(string));
    }
}
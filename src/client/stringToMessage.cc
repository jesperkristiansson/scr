#include "client/stringToMessage.h"

#include "common/messages.h"

#include <iostream>
#include <sstream>
#include <utility>

namespace{  //file private
    using MessageCreator = MessagePointer (*)(std::stringstream);

    MessagePointer JoinCreator(std::stringstream ss){
        std::string room;
        ss >> room;
        if(ss.fail()){
            std::cerr << "command /join requires an argument" << std::endl;
            return MessagePointer();
        }
        return MessagePointer(new JoinMessage(room));
    }

    MessagePointer QuitCreator(std::stringstream ss [[maybe_unused]]){
        return MessagePointer(new QuitMessage());
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

MessagePointer string_to_message(const std::string &string){
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
        return MessagePointer();
    } else{
        return MessagePointer(new MessageMessage(string));
    }
}
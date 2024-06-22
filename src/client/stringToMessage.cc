#include "client/stringToMessage.h"

#include "common/clientMessages.h"

#include <iostream>
#include <sstream>
#include <utility>

namespace{  //file private
    using MessageCreator = Message::MessagePointer (*)(std::stringstream);

    Message::MessagePointer JoinCreator(std::stringstream ss){
        std::string room;
        ss >> room;
        if(ss.fail()){
            std::cerr << "command /join requires an argument" << std::endl;
            return Message::MessagePointer();
        }
        return Message::MessagePointer(new JoinMessage(room));
    }

    Message::MessagePointer QuitCreator(std::stringstream ss [[maybe_unused]]){
        return Message::MessagePointer(new QuitMessage());
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

Message::MessagePointer string_to_message(const std::string &string){
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
        return Message::MessagePointer();
    } else{
        return Message::MessagePointer(new MessageMessage(string));
    }
}
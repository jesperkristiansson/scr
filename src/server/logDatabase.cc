#include "server/logDatabase.h"

#include <iostream>
#include <fstream>

LogDatabase::LogDatabase(const std::string &db_dir) : base_path(db_dir) {
    if(!std::filesystem::create_directories(base_path)){
        //directories were not created
    }
}

bool LogDatabase::add_message(const std::string &msg, const std::string &room){
    std::filesystem::path file_path = base_path / room;
    std::ofstream file(file_path, std::ios::app);

    file << msg << '\n';
    return true;
}

std::vector<std::string> LogDatabase::get_messages(const std::string &room){
    std::filesystem::path file_path = base_path / room;
    std::ifstream file(file_path);

    std::vector<std::string> messages;
    std::string line;
    while(getline(file, line)){
        messages.push_back(line);
    }
    return messages;
}
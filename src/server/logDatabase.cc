#include "server/logDatabase.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cassert>

namespace{
    constexpr char SEPARATOR = ';';
    constexpr const char *fmt = "%Y %b %d %H:%M:%S";
}

LogDatabase::LogDatabase(const std::string &db_dir) : base_path(db_dir) {
    if(!std::filesystem::create_directories(base_path)){
        //directories were not created
    }
}

bool LogDatabase::add_message(const struct message_info &msg, const std::string &room){
    std::filesystem::path file_path = base_path / room;
    std::ofstream file(file_path, std::ios::app);

    file << std::put_time(&msg.tm, fmt) << SEPARATOR << msg.user_name << SEPARATOR << msg.msg << '\n';
    return true;
}

std::vector<struct message_info> LogDatabase::get_messages(const std::string &room){
    std::filesystem::path file_path = base_path / room;
    std::ifstream file(file_path);

    std::vector<struct message_info> messages;
    std::string line;
    while(getline(file, line)){
        size_t name_pos = line.find(SEPARATOR);
        assert(name_pos != std::string::npos);
        size_t msg_pos = line.find(SEPARATOR, name_pos + 1);
        assert(msg_pos != std::string::npos);
        std::string time_str = line.substr(0, name_pos);
        std::string name_str = line.substr(name_pos + 1, msg_pos);
        std::string msg_str = line.substr(msg_pos + 1);

        struct message_info mi;
        mi.msg = msg_str;
        mi.user_name = name_str;
        std::istringstream(time_str) >> std::get_time(&mi.tm, fmt);
        messages.push_back(mi);
    }
    return messages;
}
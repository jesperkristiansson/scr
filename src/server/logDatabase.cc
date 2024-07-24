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

    file << std::put_time(&msg.tm, fmt) << SEPARATOR << msg.msg << '\n';
    return true;
}

std::vector<struct message_info> LogDatabase::get_messages(const std::string &room){
    std::filesystem::path file_path = base_path / room;
    std::ifstream file(file_path);

    std::vector<struct message_info> messages;
    std::string line;
    while(getline(file, line)){
        size_t pos = line.find(SEPARATOR);
        assert(pos != std::string::npos);
        std::string time_str = line.substr(0, pos);
        std::string msg_str = line.substr(pos + 1);

        struct message_info mi;
        mi.msg = msg_str;
        std::istringstream(time_str) >> std::get_time(&mi.tm, fmt);
        messages.push_back(mi);
    }
    return messages;
}
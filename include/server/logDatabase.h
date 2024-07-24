#ifndef LOG_DATABASE_H
#define LOG_DATABASE_H

#include <filesystem>
#include <string>
#include <vector>
#include <ctime>

struct message_info{
    std::string msg;
    std::tm tm;
};

class LogDatabase{
public:
    LogDatabase(const std::string &db_dir);
    bool add_message(const struct message_info &msg, const std::string &room);
    std::vector<struct message_info> get_messages(const std::string &room);
private:
    const std::filesystem::path base_path;
};

#endif
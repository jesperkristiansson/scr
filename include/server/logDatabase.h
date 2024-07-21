#ifndef LOG_DATABASE_H
#define LOG_DATABASE_H

#include <filesystem>
#include <string>
#include <vector>

class LogDatabase{
public:
    LogDatabase(const std::string &db_dir);
    bool add_message(const std::string &msg, const std::string &room);
    std::vector<std::string> get_messages(const std::string &room);
private:
    std::filesystem::path base_path;
};

#endif
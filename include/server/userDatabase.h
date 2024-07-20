#ifndef USER_DATABASE_H
#define USER_DATABASE_H

#include <string>
#include <fstream>

class UserDatabase{
public:
    UserDatabase(const std::string &db_file);
    bool user_exists(const std::string &username, const std::string &password);
    bool insert_user(const std::string &username, const std::string &password);
private:
    std::fstream fs;

    bool user_exists_hashed(const std::string &username, const std::string &hash);
};

#endif
#include "server/userDatabase.h"

#include <iostream>
#include <cassert>

namespace{
    constexpr char SEPARATOR = ':';

    //replace with cryptographic hashing function, e.g. from openSSL
    std::string hash(const std::string &input){
        std::string output(input);
        return output;
    }
}

UserDatabase::UserDatabase(const std::string &db_file) : fs(db_file, std::ios::in | std::ios::out | std::ios::app) {}

bool UserDatabase::user_exists_hashed(const std::string &username, const std::string &hash){
    //go to start of file
    fs.seekg(0);

    std::string line;
    while(getline(fs, line)){
        size_t pos = line.find(SEPARATOR);
        assert(pos != std::string::npos);

        //compare username, then hash
        if(username.compare(0, pos, line) == 0 && hash.compare(pos + 1, std::string::npos, line) == 0){
            return true;
        }
    }

    fs.clear(std::ios_base::eofbit);
    return false;
}

bool UserDatabase::user_exists(const std::string &username, const std::string &password){
    std::string hashed = hash(password);
    return user_exists_hashed(username, hashed);
}

bool UserDatabase::insert_user(const std::string &username, const std::string &password){
    std::string hashed = hash(password);
    if(user_exists_hashed(username, hashed)){
        return false;
    }

    fs << username << SEPARATOR << hashed << '\n';
    return true;
}
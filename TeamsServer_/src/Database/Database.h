#ifndef DATABASE_H
#define DATABASE_H

#include "../Structs/User.h"
#include "../includes.h";

class Database {
public:
    Database(const std::string& db_url, const std::string& db_name) : db_url_(db_url), db_name_(db_name)
    {
        init_database();
    }

    bool add_user(const std::string& username, const std::string& password);
    std::optional<User> authenticate_user(const std::string& username, const std::string& password);
    std::optional<User> get_user_by_token(const std::string& token);

private:
    std::string db_url_;
    std::string db_name_;

    void init_database();

    std::string hash_password(const std::string& password);
    std::string generate_token();
};

#endif
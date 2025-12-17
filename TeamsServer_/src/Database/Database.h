#pragma once
#include <pqxx/pqxx>
#include <memory>
#include <string>
#include <optional>
#include "../Structs/User.h"
#include "ConnectionPool.h"

class Database {
public:
    static Database& instance() {
        static Database instance_;
        return instance_;
    }

    void init_from_env();  // Initialise db_url, db_name, tables, pool

    bool add_user(const std::string& username, const std::string& password);
    std::optional<User> authenticate_user(const std::string& username, const std::string& password);
    std::optional<User> get_user_by_token(const std::string& token);

private:
    Database() { init_from_env(); };
    ~Database() = default;
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    void init_database(); // Création DB & tables
    std::string hash_password(const std::string& password);
    std::string generate_token();

    std::string db_url_;
    std::string db_name_;
    std::string table_users_;
    std::string table_channels_;
    std::string table_channel_members_;
    std::string table_messages_;

    std::unique_ptr<ConnectionPool> pool_;
};
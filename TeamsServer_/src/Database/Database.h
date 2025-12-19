#ifndef DATABASE_H
#define DATABASE_H

#include "../Structs/User.h"
#include "../includes.h"
#include "ConnectionPool.h"

class Database {
public:
  static Database &instance() {
    static Database instance_;
    return instance_;
  }

  bool add_user(const std::string &username, const std::string &password);
  std::optional<User> authenticate_user(const std::string &username,
                                        const std::string &password);
  std::optional<User> get_user_by_token(const std::string &token);

private:
  Database() { init_from_env(); };
  ~Database() = default;

  void init_database(); // Création tables
  void init_from_env(); // Création DB si absente

  std::string hash_password(const std::string &password);
  std::string generate_token();


  // Variables du .env (Tout ce qui concerne la DB)
  std::string db_url_;
  std::string db_name_;
  std::string table_users_;
  std::string table_channels_;
  std::string table_channel_members_;
  std::string table_messages_;

  std::unique_ptr<ConnectionPool> pool_;

  // Supprime copie et déplacement pour empêcher la duplication du singleton
  Database(const Database &) = delete;
  Database(Database &&) = delete;
  Database &operator=(const Database &) = delete;
  Database &operator=(Database &&) = delete;
};

#endif
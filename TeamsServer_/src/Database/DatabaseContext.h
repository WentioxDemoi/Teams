#ifndef DATABASECONTEXT_H
#define DATABASECONTEXT_H

#include "../includes.h"
#include "ConnectionPool.h"

class DatabaseContext {
public:
  static DatabaseContext &instance();

  std::shared_ptr<pqxx::connection> acquire();
  void release(std::shared_ptr<pqxx::connection> conn);

  const std::string &users_table() const;
  const std::string &channels_table() const;
  const std::string &channel_members_table() const;
  const std::string &messages_table() const;

private:
  DatabaseContext();
  void init_from_env();

  std::string db_url_;
  std::string db_name_;

  std::string table_users_;
  std::string table_channels_;
  std::string table_channel_members_;
  std::string table_messages_;

  std::unique_ptr<ConnectionPool> pool_;

  DatabaseContext(const DatabaseContext &) = delete;
  DatabaseContext(DatabaseContext &&) = delete;
  DatabaseContext &operator=(const DatabaseContext &) = delete;
  DatabaseContext &operator=(DatabaseContext &&) = delete;
};

#endif
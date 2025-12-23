#include "DatabaseContext.h"

DatabaseContext &DatabaseContext::instance() {
  static DatabaseContext instance;
  return instance;
}

DatabaseContext::DatabaseContext() { init_from_env(); }

void DatabaseContext::init_from_env() {
  const char *env_db_url = std::getenv("DB_URL");
  const char *env_db_name = std::getenv("DB_NAME");
  if (!env_db_url || !env_db_name)
    throw std::runtime_error("DB_URL ou DB_NAME non défini");

  db_url_ = env_db_url;
  db_name_ = env_db_name;

  table_users_ = std::getenv("TABLE_USERS") ?: "users";
  table_channels_ = std::getenv("TABLE_CHANNELS") ?: "channels";
  table_channel_members_ =
      std::getenv("TABLE_CHANNEL_MEMBERS") ?: "channel_members";
  table_messages_ = std::getenv("TABLE_MESSAGES") ?: "messages";

  int nb_conn = std::getenv("NB_CONN") ? std::stoi(std::getenv("NB_CONN")) : 4;

  const std::string url = db_url_ + "/" + db_name_;
  pool_ = std::make_unique<ConnectionPool>(url, nb_conn);
}

std::shared_ptr<pqxx::connection> DatabaseContext::acquire() {
  return pool_->get_connection();
}

void DatabaseContext::release(std::shared_ptr<pqxx::connection> conn) {
  pool_->return_connection(conn);
}

const std::string &DatabaseContext::users_table() const { return table_users_; }
const std::string &DatabaseContext::channels_table() const {
  return table_channels_;
}
const std::string &DatabaseContext::channel_members_table() const {
  return table_channel_members_;
}
const std::string &DatabaseContext::messages_table() const {
  return table_messages_;
}
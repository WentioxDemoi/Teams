// utils/Config.h
#ifndef CONFIG_H
#define CONFIG_H

#include "../includes.h"

/**
 * @class Config
 * @brief Gestionnaire singleton de la configuration de l'application.
 *
 * Centralise les paramètres pour la base de données, le pool de workers, les
 * tables, les tokens d'authentification et la configuration serveur/SSL.
 */
class Config {
public:
  static Config &instance();

  // Database config
  std::string db_url() const { return db_url_; }
  std::string db_name() const { return db_name_; }
  int db_pool_size() const { return db_pool_size_; }
  // Workers config

  int worker_pool_size() const { return worker_pool_size_; }

  // Table names
  std::string table_users() const { return table_users_; }
  std::string table_channels() const { return table_channels_; }
  std::string table_channel_members() const { return table_channel_members_; }
  std::string table_messages() const { return table_messages_; }

  std::string
  time_point_to_string(const std::chrono::system_clock::time_point &tp);
  std::chrono::system_clock::time_point
  string_to_time_point(const std::string &str);

  // Token config
  int token_expiry_minutes() const { return token_expiry_minutes_; }
  std::chrono::system_clock::time_point token_expiry_time() const {
    return std::chrono::system_clock::now() +
           std::chrono::minutes(token_expiry_minutes_);
  }

  std::chrono::system_clock::time_point time() const {
    return std::chrono::system_clock::now();
  }
  // Server config (pour plus tard)
  int server_port() const { return server_port_; }
  std::string ssl_cert_path() const { return ssl_cert_path_; }
  std::string ssl_key_path() const { return ssl_key_path_; }

private:
  Config();
  ~Config() = default;

  Config(const Config &) = delete;
  Config(Config &&) = delete;
  Config &operator=(const Config &) = delete;
  Config &operator=(Config &&) = delete;

  void load_from_env();
  std::string get_env(const char *name, const std::string &default_value = "");
  int get_env_int(const char *name, int default_value);

  // Database
  std::string db_url_;
  std::string db_name_;
  int db_pool_size_;

  // Workers
  int worker_pool_size_;

  // Tables
  std::string table_users_;
  std::string table_channels_;
  std::string table_channel_members_;
  std::string table_messages_;

  // Token
  int token_expiry_minutes_;

  // Server
  int server_port_;
  std::string ssl_cert_path_;
  std::string ssl_key_path_;
};

#endif
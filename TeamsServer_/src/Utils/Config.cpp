// utils/Config.cpp
#include "Config.h"

Config &Config::instance() {
  static Config instance;
  return instance;
}

Config::Config() { load_from_env(); }

void Config::load_from_env() {
  std::cout << "[Config] Loading configuration from environment..."
            << std::endl;

  // Database (required)
  db_url_ = get_env("DB_URL");
  db_name_ = get_env("DB_NAME");

  if (db_url_.empty() || db_name_.empty()) {
    throw std::runtime_error(
        "DB_URL and DB_NAME are required environment variables");
  }

  db_pool_size_ = get_env_int("DB_POOL_SIZE", 4);

  worker_pool_size_ = get_env_int("NB_THREADS", 4);

  // Tables (with defaults)
  table_users_ = get_env("TABLE_USERS", "users");
  table_channels_ = get_env("TABLE_CHANNELS", "channels");
  table_channel_members_ = get_env("TABLE_CHANNEL_MEMBERS", "channel_members");
  table_messages_ = get_env("TABLE_MESSAGES", "messages");

  // Token
  token_expiry_minutes_ = get_env_int("TOKEN_EXPIRY_MINUTES", 1);

  // Server
  server_port_ = get_env_int("SERVER_PORT", 8080);
  ssl_cert_path_ = get_env("SSL_CERT_PATH", "server.crt");
  ssl_key_path_ = get_env("SSL_KEY_PATH", "server.key");

  std::cout << "[Config] Configuration loaded successfully" << std::endl;
}

std::string Config::get_env(const char *name,
                            const std::string &default_value) {
  const char *value = std::getenv(name);
  return value ? std::string(value) : default_value;
}

int Config::get_env_int(const char *name, int default_value) {
  const char *value = std::getenv(name);
  if (!value)
    return default_value;

  try {
    return std::stoi(value);
  } catch (...) {
    std::cerr << "[Config] Warning: Invalid integer for " << name
              << ", using default: " << default_value << std::endl;
    return default_value;
  }
}

std::string
Config::time_point_to_string(const std::chrono::system_clock::time_point &tp) {
  std::time_t t = std::chrono::system_clock::to_time_t(tp);
  std::tm tm = *std::gmtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

std::chrono::system_clock::time_point
Config::string_to_time_point(const std::string &str) {
  std::tm tm{};
  std::istringstream iss(str);
  iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

  if (iss.fail()) {
    throw std::runtime_error("Invalid time format");
  }

  std::time_t t = std::mktime(&tm);
  return std::chrono::system_clock::from_time_t(t);
}
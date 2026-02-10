// tests/TestConfig.cpp
#include "../../src/Utils/Config.h"
#include "../includes.h"

TEST(ConfigTest, SingletonInstance) {
  Config &c1 = Config::instance();
  Config &c2 = Config::instance();
  EXPECT_EQ(&c1, &c2);
}

TEST(ConfigTest, DatabaseConfig) {
  Config &config = Config::instance();
  EXPECT_EQ(config.db_url(), "postgresql://myuser:mypassword@db:5432");
  EXPECT_EQ(config.db_name(), "Teams");
  EXPECT_EQ(config.worker_pool_size(), 4);
}

TEST(ConfigTest, TokenConfig) {
  Config &config = Config::instance();
  EXPECT_EQ(config.token_expiry_minutes(), 1);

  auto now = std::chrono::system_clock::now();
  auto expiry = config.token_expiry_time();
  EXPECT_GT(expiry, now);
}

TEST(ConfigTest, TableNames) {
  Config &config = Config::instance();
  EXPECT_EQ(config.table_users(), "users");
  EXPECT_EQ(config.table_channels(), "channels");
  EXPECT_EQ(config.table_channel_members(), "channel_members");
  EXPECT_EQ(config.table_messages(), "messages");
}

TEST(ConfigTest, TimeConversion) {
  Config &config = Config::instance();
  auto now = std::chrono::system_clock::now();
  std::string s = config.time_point_to_string(now);
  auto tp = config.string_to_time_point(s);

  // tolérance 1 seconde pour les arrondis de std::mktime
  auto diff =
      std::chrono::duration_cast<std::chrono::seconds>(tp - now).count();
  EXPECT_LE(std::abs(diff), 1);
}
#include "Infrastructure/ConnectionPool.h"

#include <gtest/gtest.h>

TEST(ConnectionPoolTest, CanAcquireAndReturnConnectionWhenDatabaseIsConfigured) {
  const char* connectionString = std::getenv("TEST_DATABASE_URL");
  if (connectionString == nullptr || std::string(connectionString).empty()) {
    GTEST_SKIP() << "TEST_DATABASE_URL n'est pas configure";
  }

  ConnectionPool pool(connectionString, 1);
  auto connection = pool.get_connection();
  ASSERT_TRUE(connection);
  EXPECT_TRUE(connection->is_open());

  pool.return_connection(connection);
  auto reusedConnection = pool.get_connection();
  ASSERT_TRUE(reusedConnection);
  EXPECT_TRUE(reusedConnection->is_open());
  pool.return_connection(reusedConnection);
}

TEST(ConnectionPoolTest, InvalidConnectionStringDoesNotThrowFromConstructor) {
  EXPECT_NO_THROW(ConnectionPool pool("host=127.0.0.1 port=1 dbname=missing", 1));
}
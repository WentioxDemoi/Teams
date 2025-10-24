#include <gtest/gtest.h>
#include "../DatabaseManager.h"

TEST(DatabaseManagerTest, CanConnectToDatabase) {
    DatabaseManager db;
    EXPECT_TRUE(db.open("test.db"));
}
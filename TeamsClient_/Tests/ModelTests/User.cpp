#include <QJsonObject>
#include "User.h"
#include <gtest/gtest.h>

// --------------------
// Constructeurs
// --------------------

TEST(UserTest, DefaultConstructor) {
    User user;

    EXPECT_EQ(user.id(), 0);
    EXPECT_TRUE(user.email().isEmpty());
    EXPECT_TRUE(user.username().isEmpty());
    EXPECT_TRUE(user.token().isEmpty());
    EXPECT_TRUE(user.status().isEmpty());
    EXPECT_FALSE(user.isValid());
}

TEST(UserTest, ParameterizedConstructor) {
    User user(1, "test@mail.com", "john", "abc123", "active");

    EXPECT_EQ(user.id(), 1);
    EXPECT_EQ(user.email(), "test@mail.com");
    EXPECT_EQ(user.username(), "john");
    EXPECT_EQ(user.token(), "abc123");
    EXPECT_EQ(user.status(), "active");
    EXPECT_TRUE(user.isValid());
}

// --------------------
// Setters / Getters
// --------------------

TEST(UserTest, SettersAndGetters) {
    User user;

    user.setId(42);
    user.setEmail("user@mail.com");
    user.setUsername("alice");
    user.setToken("token");
    user.setStatus("online");

    EXPECT_EQ(user.id(), 42);
    EXPECT_EQ(user.email(), "user@mail.com");
    EXPECT_EQ(user.username(), "alice");
    EXPECT_EQ(user.token(), "token");
    EXPECT_EQ(user.status(), "online");
}

// --------------------
// isValid()
// --------------------

TEST(UserTest, IsValidRequiresIdAndEmail) {
    User user;

    EXPECT_FALSE(user.isValid());

    user.setId(1);
    EXPECT_FALSE(user.isValid());

    user.setEmail("valid@mail.com");
    EXPECT_TRUE(user.isValid());
}

// --------------------
// toJson()
// --------------------

TEST(UserTest, ToJsonSerialization) {
    User user(7, "json@mail.com", "jsonuser", "tok123", "busy");

    QJsonObject json = user.toJson();

    EXPECT_EQ(json["id"].toInt(), 7);
    EXPECT_EQ(json["email"].toString(), "json@mail.com");
    EXPECT_EQ(json["username"].toString(), "jsonuser");
    EXPECT_EQ(json["token"].toString(), "tok123");
    EXPECT_EQ(json["status"].toString(), "busy");
}

// --------------------
// fromJson()
// --------------------

TEST(UserTest, FromJsonDeserialization) {
    QJsonObject json;
    json["id"] = 3;
    json["email"] = "fromjson@mail.com";
    json["username"] = "bob";
    json["token"] = "zzz";
    json["status"] = "offline";

    User user = User::fromJson(json);

    EXPECT_EQ(user.id(), 3);
    EXPECT_EQ(user.email(), "fromjson@mail.com");
    EXPECT_EQ(user.username(), "bob");
    EXPECT_EQ(user.token(), "zzz");
    EXPECT_EQ(user.status(), "offline");
    EXPECT_TRUE(user.isValid());
}

TEST(UserTest, FromJsonWithMissingFields) {
    QJsonObject json;
    json["email"] = "partial@mail.com";

    User user = User::fromJson(json);

    EXPECT_EQ(user.id(), 0);
    EXPECT_EQ(user.email(), "partial@mail.com");
    EXPECT_FALSE(user.isValid());
}

// --------------------
// Robustesse types JSON
// --------------------

TEST(UserTest, FromJsonWithInvalidTypes) {
    QJsonObject json;
    json["id"] = "not_an_int";
    json["email"] = 123;
    json["username"] = true;

    User user = User::fromJson(json);

    EXPECT_EQ(user.id(), 0);
    EXPECT_TRUE(user.email().isEmpty());
    EXPECT_TRUE(user.username().isEmpty());
    EXPECT_FALSE(user.isValid());
}
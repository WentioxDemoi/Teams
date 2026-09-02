#include "Infrastructure/QueryBuilder.h"

#include <gtest/gtest.h>

TEST(QueryBuilderTest, BuildsSelectWithFiltersJoinsOrderingAndPagination) {
  QueryBuilder builder;

  const auto query = builder.select({"u.id", "u.email"})
                         .from("users u")
                         .join("contacts c", "c.user_id = u.id")
                         .left_join("profiles p", "p.user_id = u.id")
                         .where("u.active", "=", "$1")
                         .and_where("u.role", "=", "$2")
                         .or_where("u.email", "LIKE", "$3")
                         .order_by("u.created_at", "DESC")
                         .order_by("u.email")
                         .limit(10)
                         .offset(20)
                         .build();

  EXPECT_EQ(query,
            "SELECT u.id, u.email FROM users u "
            "JOIN contacts c ON c.user_id = u.id "
            "LEFT JOIN profiles p ON p.user_id = u.id "
            "WHERE u.active = $1 AND u.role = $2 OR u.email LIKE $3 "
            "ORDER BY u.created_at DESC, u.email ASC LIMIT 10 OFFSET 20");
}

TEST(QueryBuilderTest, UsesDefaultSelectColumn) {
  EXPECT_EQ(QueryBuilder().select().from("users").build(), "SELECT * FROM users");
}

TEST(QueryBuilderTest, BuildsInsertWithColumnsAndReturning) {
  QueryBuilder builder;

  const auto query = builder.insert_into("users", {"email", "password_hash"})
                         .values({"$1", "$2"})
                         .returning({"id", "created_at"})
                         .build();

  EXPECT_EQ(query,
            "INSERT INTO users (email, password_hash) VALUES ($1, $2) "
            "RETURNING id, created_at");
}

TEST(QueryBuilderTest, BuildsInsertWithoutOptionalClauses) {
  QueryBuilder builder;

  EXPECT_EQ(builder.insert_into("users").values({"$1"}).build(), "INSERT INTO users VALUES ($1)");
}

TEST(QueryBuilderTest, BuildsUpdateWithMultipleAssignmentsFiltersAndReturning) {
  QueryBuilder builder;

  const auto query = builder.update("users")
                         .set("email", "$1")
                         .set("active", "$2")
                         .where("id", "=", "$3")
                         .returning({"id"})
                         .build();

  EXPECT_EQ(query,
            "UPDATE users SET email = $1, active = $2 WHERE id = $3 "
            "RETURNING id");
}

TEST(QueryBuilderTest, BuildsDeleteWithFiltersAndReturning) {
  QueryBuilder builder;

  EXPECT_EQ(builder.delete_from("users").where("id", "=", "$1").returning({"id"}).build(),
            "DELETE FROM users WHERE id = $1 RETURNING id");
}

TEST(QueryBuilderTest, ResetClearsAllClauses) {
  QueryBuilder builder;
  builder.select({"id"})
      .from("users")
      .where("id", "=", "$1")
      .join("profiles", "profiles.user_id = users.id")
      .order_by("id")
      .limit(1)
      .offset(2);

  builder.reset();

  EXPECT_EQ(builder.select().from("sessions").build(), "SELECT * FROM sessions");
}
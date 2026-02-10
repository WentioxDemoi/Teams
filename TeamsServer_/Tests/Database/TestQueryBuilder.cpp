#include "../../src/Infrastructure/Database/QueryBuilder.h"
#include "../includes.h"

TEST(QueryBuilderTest, SelectAllFromTable) {
  QueryBuilder qb;

  std::string sql = qb.select().from("users").build();

  EXPECT_EQ(sql, "SELECT * FROM users");
}

TEST(QueryBuilderTest, SelectSpecificColumns) {
  QueryBuilder qb;

  std::string sql = qb.select({"id", "email"}).from("users").build();

  EXPECT_EQ(sql, "SELECT id, email FROM users");
}

TEST(QueryBuilderTest, WhereAndClause) {
  QueryBuilder qb;

  std::string sql = qb.select()
                        .from("users")
                        .where("email", "=", ":email")
                        .and_where("status", "=", ":status")
                        .build();

  EXPECT_EQ(sql,
            "SELECT * FROM users WHERE email = :email AND status = :status");
}

TEST(QueryBuilderTest, OrWhereClause) {
  QueryBuilder qb;

  std::string sql = qb.select()
                        .from("users")
                        .where("username", "=", ":username")
                        .or_where("email", "=", ":email")
                        .build();

  EXPECT_EQ(sql,
            "SELECT * FROM users WHERE username = :username OR email = :email");
}

TEST(QueryBuilderTest, JoinClauses) {
  QueryBuilder qb;

  std::string sql = qb.select({"users.id", "roles.name"})
                        .from("users")
                        .join("roles", "users.role_id = roles.id")
                        .build();

  EXPECT_EQ(sql, "SELECT users.id, roles.name FROM users "
                 "JOIN roles ON users.role_id = roles.id");
}

TEST(QueryBuilderTest, OrderLimitOffset) {
  QueryBuilder qb;

  std::string sql = qb.select()
                        .from("users")
                        .order_by("created_at", "DESC")
                        .limit(10)
                        .offset(20)
                        .build();

  EXPECT_EQ(sql,
            "SELECT * FROM users ORDER BY created_at DESC LIMIT 10 OFFSET 20");
}

TEST(QueryBuilderTest, InsertQuery) {
  QueryBuilder qb;

  std::string sql = qb.insert_into("users", {"email", "password"})
                        .values({":email", ":password"})
                        .build();

  EXPECT_EQ(sql,
            "INSERT INTO users (email, password) VALUES (:email, :password)");
}

TEST(QueryBuilderTest, InsertReturning) {
  QueryBuilder qb;

  std::string sql = qb.insert_into("users", {"email"})
                        .values({":email"})
                        .returning({"id"})
                        .build();

  EXPECT_EQ(sql, "INSERT INTO users (email) VALUES (:email) RETURNING id");
}

TEST(QueryBuilderTest, UpdateQuery) {
  QueryBuilder qb;

  std::string sql = qb.update("users")
                        .set("status", ":status")
                        .where("id", "=", ":id")
                        .build();

  EXPECT_EQ(sql, "UPDATE users SET status = :status WHERE id = :id");
}

TEST(QueryBuilderTest, DeleteQuery) {
  QueryBuilder qb;

  std::string sql = qb.delete_from("users").where("id", "=", ":id").build();

  EXPECT_EQ(sql, "DELETE FROM users WHERE id = :id");
}

TEST(QueryBuilderTest, ResetClearsState) {
  QueryBuilder qb;

  qb.select().from("users").where("id", "=", ":id");
  qb.reset();

  std::string sql = qb.select().from("roles").build();

  EXPECT_EQ(sql, "SELECT * FROM roles");
}

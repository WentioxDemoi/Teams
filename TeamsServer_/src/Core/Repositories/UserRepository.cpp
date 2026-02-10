#include "UserRepository.h"

User UserRepository::row_to_user(const pqxx::row &row) {
  User user;
  user.uuid = row["uuid"].as<std::string>();
  user.username = row["username"].as<std::string>();
  user.email = row["email"].as<std::string>();
  user.password_hash = row["password_hash"].as<std::string>();
  user.status = row["status"].as<std::string>();
  user.token = row["token"].as<std::string>();

  user.created_at =
      config_.string_to_time_point(row["created_at"].as<std::string>());
  user.last_seen =
      config_.string_to_time_point(row["last_seen"].as<std::string>());
  user.token_expires_at =
      config_.string_to_time_point(row["token_expires_at"].as<std::string>());

  // Debug timestamps
  std::cout << "[DEBUG row_to_user] uuid: " << user.uuid
            << ", created_at: " << row["created_at"].c_str()
            << ", last_seen: " << row["last_seen"].c_str()
            << ", token_expires_at: " << row["token_expires_at"].c_str()
            << std::endl;

  return user;
}

std::optional<User> UserRepository::find_by_uuid(const std::string &uuid) {
  auto conn = databaseManager_.acquire_connection();
  try {
    std::cout << "[DEBUG find_by_uuid] Called with uuid: " << uuid << std::endl;

    QueryBuilder qb;
    std::string query = qb.select()
                            .from(config_.table_users())
                            .where("uuid", "=", "$1")
                            .build();
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params(query, uuid);
    databaseManager_.release_connection(conn);

    std::cout << "[DEBUG find_by_uuid] Result rows: " << result.size()
              << std::endl;

    if (result.empty())
      return std::nullopt;
    return row_to_user(result[0]);
  } catch (const std::exception &e) {
    databaseManager_.release_connection(conn);
    std::cerr << "[ERROR find_by_uuid] Exception: " << e.what() << std::endl;
    throw;
  }
}

std::optional<User> UserRepository::find_by_email(const std::string &email) {
  auto conn = databaseManager_.acquire_connection();
  try {
    std::cout << "[DEBUG find_by_email] Called with email: " << email
              << std::endl;

    QueryBuilder qb;
    std::string query = qb.select()
                            .from(config_.table_users())
                            .where("email", "=", "$1")
                            .build();
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params(query, email);
    databaseManager_.release_connection(conn);

    std::cout << "[DEBUG find_by_email] Result rows: " << result.size()
              << std::endl;

    if (result.empty())
      return std::nullopt;
    return row_to_user(result[0]);
  } catch (const std::exception &e) {
    databaseManager_.release_connection(conn);
    std::cerr << "[ERROR find_by_email] Exception: " << e.what() << std::endl;
    throw;
  }
}

std::optional<User> UserRepository::find_by_token(const std::string &token) {
  auto conn = databaseManager_.acquire_connection();
  try {
    std::cout << "[DEBUG find_by_token] Called with token: " << token
              << std::endl;

    QueryBuilder qb;
    std::string query = qb.select()
                            .from(config_.table_users())
                            .where("token", "=", "$1")
                            .and_where("token_expires_at", ">", "NOW()")
                            .build();

    std::cout << "[DEBUG find_by_token] SQL Query: " << query << std::endl;

    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params(query, token);

    std::cout << "[DEBUG find_by_token] Result rows: " << result.size()
              << std::endl;
    for (const auto &row : result) {
      std::cout << "[DEBUG find_by_token] Row token: " << row["token"].c_str()
                << ", created_at: " << row["created_at"].c_str()
                << ", last_seen: " << row["last_seen"].c_str()
                << ", token_expires_at: " << row["token_expires_at"].c_str()
                << std::endl;
    }

    databaseManager_.release_connection(conn);

    if (result.empty()) {
      std::cout << "[DEBUG find_by_token] No valid token found or token expired"
                << std::endl;
      return std::nullopt;
    }

    return row_to_user(result[0]);
  } catch (const std::exception &e) {
    std::cerr << "[ERROR find_by_token] Exception: " << e.what() << std::endl;
    databaseManager_.release_connection(conn);
    throw;
  }
}

bool UserRepository::create(const User &user) {
  auto conn = databaseManager_.acquire_connection();
  try {
    std::cout << "[DEBUG create] Creating user: " << user.uuid
              << ", token_expires_at: "
              << config_.time_point_to_string(user.token_expires_at)
              << std::endl;

    QueryBuilder qb;
    std::string query =
        qb.insert_into(config_.table_users(),
                       {"uuid", "username", "email", "password_hash", "token",
                        "token_expires_at", "status", "last_seen",
                        "created_at"})
            .values({"$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9"})
            .returning({"uuid"})
            .build();

    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params(
        query, user.uuid, user.username, user.email, user.password_hash,
        user.token, config_.time_point_to_string(user.token_expires_at),
        user.status, config_.time_point_to_string(user.last_seen),
        config_.time_point_to_string(user.created_at));

    txn.commit();
    databaseManager_.release_connection(conn);

    return !result.empty();
  } catch (...) {
    databaseManager_.release_connection(conn);
    throw;
  }
}

bool UserRepository::update(const User &user) {
  auto conn = databaseManager_.acquire_connection();
  try {
    std::cout << "[DEBUG update] Updating user: " << user.uuid
              << ", token_expires_at: "
              << config_.time_point_to_string(user.token_expires_at)
              << std::endl;

    QueryBuilder qb;
    std::string query = qb.update(config_.table_users())
                            .set("username", "$1")
                            .set("email", "$2")
                            .set("password_hash", "$3")
                            .set("token", "$4")
                            .set("token_expires_at", "$5")
                            .set("status", "$6")
                            .set("last_seen", "NOW()")
                            .where("uuid", "=", "$7")
                            .build();

    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params(
        query, user.username, user.email, user.password_hash, user.token,
        config_.time_point_to_string(user.token_expires_at), user.status,
        user.uuid);

    txn.commit();
    databaseManager_.release_connection(conn);

    return result.affected_rows() == 1;
  } catch (const std::exception &e) {
    databaseManager_.release_connection(conn);
    std::cerr << "[ERROR update] Exception: " << e.what() << std::endl;
    throw;
  }
}

bool UserRepository::delete_user(const std::string &uuid) {
  auto conn = databaseManager_.acquire_connection();
  try {
    std::cout << "[DEBUG delete_user] Deleting user: " << uuid << std::endl;

    QueryBuilder qb;
    std::string query =
        qb.delete_from(config_.table_users()).where("uuid", "=", "$1").build();

    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params(query, uuid);
    txn.commit();

    databaseManager_.release_connection(conn);

    return result.affected_rows() == 1;
  } catch (const std::exception &e) {
    databaseManager_.release_connection(conn);
    std::cerr << "[ERROR delete_user] Exception: " << e.what() << std::endl;
    throw;
  }
}
#include "UserRepository.h"

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

    auto check = txn.exec("SELECT token_expires_at, NOW(), token_expires_at > NOW() FROM " 
                       + config_.table_users() + " WHERE token = '" + token + "'");
if (!check.empty()) {
    std::cout << "[DEBUG] expires_at: " << check[0][0].c_str()
              << " NOW: "              << check[0][1].c_str()
              << " valid: "            << check[0][2].c_str() << std::endl;
}

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

User UserRepository::row_to_user(const pqxx::row &row) {
  User user;
  user.uuid           = row["uuid"].as<std::string>();
  user.firstName     = row["first_name"].as<std::string>();
  user.lastName      = row["last_name"].as<std::string>();
  user.email          = row["email"].as<std::string>();
  user.password_hash  = row["password_hash"].as<std::string>();
  user.status         = row["status"].as<std::string>();
  user.token          = row["token"].as<std::string>();
  user.created_at     = config_.string_to_time_point(row["created_at"].as<std::string>());
  user.last_seen      = config_.string_to_time_point(row["last_seen"].as<std::string>());
  user.token_expires_at = config_.string_to_time_point(row["token_expires_at"].as<std::string>());

  std::cout << "[DEBUG row_to_user] uuid: " << user.uuid
          << ", token: " << user.token
          << ", token_expires_at: " << row["token_expires_at"].c_str()
          << std::endl;

  return user;
}

bool UserRepository::create(const User &user) {
  auto conn = databaseManager_.acquire_connection();
  try {
    QueryBuilder qb;
    std::string query =
        qb.insert_into(config_.table_users(),
                       {"uuid", "first_name", "last_name", "email",
                        "password_hash", "token", "token_expires_at",
                        "status", "last_seen", "created_at"})
            .values({"$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10"})
            .returning({"uuid"})
            .build();

    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params(
        query,
        user.uuid,
        user.firstName,
        user.lastName,
        user.email,
        user.password_hash,
        user.token,
        config_.time_point_to_string(user.token_expires_at),
        user.status,
        config_.time_point_to_string(user.last_seen),
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
    QueryBuilder qb;
    std::string query = qb.update(config_.table_users())
                            .set("first_name", "$1")
                            .set("last_name", "$2")
                            .set("email", "$3")
                            .set("password_hash", "$4")
                            .set("token", "$5")
                            .set("token_expires_at", "$6")
                            .set("status", "$7")
                            .set("last_seen", "NOW()")
                            .where("uuid", "=", "$8")
                            .build();

    pqxx::work txn(*conn);
    std::cout << "[DEBUG update] SQL: " << query << std::endl;
std::cout << "[DEBUG update] token: " << user.token 
          << ", expires: " << config_.time_point_to_string(user.token_expires_at)
          << ", uuid: " << user.uuid << std::endl;
    pqxx::result result = txn.exec_params(
        query,
        user.firstName,
        user.lastName,
        user.email,
        user.password_hash,
        user.token,
        config_.time_point_to_string(user.token_expires_at),
        user.status,
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
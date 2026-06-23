#include "UserRepository.h"
#include "../Models/User.h"

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
    return user_from_db_row(result[0]);
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
    return user_from_db_row(result[0]);
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

    return user_from_db_row(result[0]);
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


bool UserRepository::create(const User &user) {
  auto conn = databaseManager_.acquire_connection();
  try {
    QueryBuilder qb;
    std::string query =
        qb.insert_into(config_.table_users(),
                       {"uuid", "first_name", "last_name", "email",
                        "password_hash", "token", "token_expires_at",
                         "last_seen", "created_at"})
            .values({"$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9"})
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
                            .set("last_seen", "NOW()")
                            .where("uuid", "=", "$7")
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

std::vector<User> UserRepository::search_by_name(const std::string &callerUuid,
                                                  const std::string &name) {
  auto conn = databaseManager_.acquire_connection();
  std::vector<User> users;
  try {
    std::cout << "[DEBUG search_by_name] Called with name: " << name
              << ", excluding uuid: " << callerUuid << std::endl;

    // ILIKE pour insensibilité à la casse, % de chaque côté pour matcher une sous-chaîne.
    std::string pattern = "%" + name + "%";

    std::string query =
        "SELECT * FROM " + config_.table_users() +
        " WHERE (first_name ILIKE $1 OR last_name ILIKE $1 OR email ILIKE $1)"
        " AND uuid != $2"
        " LIMIT 20";

    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params(query, pattern, callerUuid);
    databaseManager_.release_connection(conn);

    std::cout << "[DEBUG search_by_name] Result rows: " << result.size()
              << std::endl;

    users.reserve(result.size());
    for (const auto &row : result) {
      users.push_back(user_from_db_row(row));
    }

    return users;
  } catch (const std::exception &e) {
    databaseManager_.release_connection(conn);
    std::cerr << "[ERROR search_by_name] Exception: " << e.what() << std::endl;
    throw;
  }
}
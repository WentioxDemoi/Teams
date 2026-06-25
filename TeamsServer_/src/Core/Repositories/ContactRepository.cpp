#include "ContactRepository.h"

bool ContactRepository::create(const Contact& contact) {
  auto conn = databaseManager_.acquire_connection();

  try {
    QueryBuilder qb;

    std::string query =
        qb.insert_into(
              config_.table_contacts(),
              {"user_uuid", "contact_uuid", "last_read_at"})
            .values({"$1", "$2", "NOW()"})
            .build();

    pqxx::work txn(*conn);

    pqxx::result result =
        txn.exec_params(
            query,
            contact.user_id,
            contact.contact_id);

    txn.commit();

    databaseManager_.release_connection(conn);

    return result.affected_rows() == 1;

  } catch (...) {
    databaseManager_.release_connection(conn);
    throw;
  }
}

bool ContactRepository::remove(
    const std::string& userUuid,
    const std::string& contactUuid) {

//   auto conn = databaseManager_.acquire_connection();

//   try {
//     QueryBuilder qb;

//     std::string query =
//         qb.delete_from(config_.table_contacts())
//             .where("user_uuid", "=", "$1")
//             .and_where("contact_uuid", "=", "$2")
//             .build();

//     pqxx::work txn(*conn);

//     pqxx::result result =
//         txn.exec_params(
//             query,
//             userUuid,
//             contactUuid);

//     txn.commit();

//     databaseManager_.release_connection(conn);

//     return result.affected_rows() == 1;

//   } catch (...) {
//     databaseManager_.release_connection(conn);
//     throw;
//   }
}

std::vector<User> ContactRepository::find_contacts(
    const std::string& userUuid) {

  auto conn = databaseManager_.acquire_connection();

  try {
    std::vector<User> users;

    std::string query = R"(
      SELECT u.*, c.last_read_at
      FROM contacts c
      JOIN users u
        ON u.uuid = c.contact_uuid
      WHERE c.user_uuid = $1
      ORDER BY u.first_name
    )";

    pqxx::work txn(*conn);

    pqxx::result result = txn.exec_params(query, userUuid);

    for (const auto& row : result) {
      User user;

      user.uuid = row["uuid"].as<std::string>();
      user.firstName = row["first_name"].as<std::string>();
      user.lastName = row["last_name"].as<std::string>();
      user.email = row["email"].as<std::string>();
      user.lastReadAt = row["last_read_at"].is_null() ? "" : row["last_read_at"].as<std::string>();


      users.push_back(user);
    }

    txn.commit();

    databaseManager_.release_connection(conn);

    return users;

  } catch (...) {
    databaseManager_.release_connection(conn);
    throw;
  }
}

bool ContactRepository::update_last_read_at(
    const std::string& userUuid,
    const std::string& contactUuid,
    const std::string& lastReadAt) {

  auto conn = databaseManager_.acquire_connection();

  try {
    QueryBuilder qb;

    std::string query =
        qb.update(config_.table_contacts())
            .set("last_read_at", "$1")
            .where("user_uuid", "=", "$2")
            .and_where("contact_uuid", "=", "$3")
            .build();

    pqxx::work txn(*conn);

    pqxx::result result =
        txn.exec_params(
            query,
            lastReadAt,
            userUuid,
            contactUuid);

    txn.commit();

    databaseManager_.release_connection(conn);

    std::cout <<" allerrr";
    return result.affected_rows() == 1;

  } catch (...) {
    databaseManager_.release_connection(conn);
    throw;
  }
}
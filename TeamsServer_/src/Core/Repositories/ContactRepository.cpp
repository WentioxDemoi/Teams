#include "ContactRepository.h"

bool ContactRepository::create(const Contact& contact) {
  auto conn = databaseManager_.acquire_connection();

  try {
    QueryBuilder qb;

    std::string query =
        qb.insert_into(
              config_.table_contacts(),
              {"user_uuid", "contact_uuid"})
            .values({"$1", "$2"})
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
    std::vector<User> contacts;

    std::string query = R"(
      SELECT u.*
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
      user.status = row["status"].as<std::string>();

      contacts.push_back(user);
    }

    txn.commit();

    databaseManager_.release_connection(conn);

    return contacts;

  } catch (...) {
    databaseManager_.release_connection(conn);
    throw;
  }
}
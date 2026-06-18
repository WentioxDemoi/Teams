#include "MessageRepository.h"
#include <iostream>
#include "../../Infrastructure/Database/QueryBuilder.h"

bool MessageRepository::save(const Message& message) {
  try {
    auto conn = databaseManager_.acquire_connection();
    pqxx::work txn(*conn);

    QueryBuilder qb;

    auto query =
        qb.insert_into(config_.table_messages(), {"id", "sender_id", "receiver_id", "chat_type",
                                                  "content", "timestamp", "is_read"})
            .values({"$1", "$2", "$3", "$4", "$5", "$6", "$7"})
            .build();

    txn.exec_params(query, message.id, message.sender_id, message.receiver_id, message.chatType,
                    message.content, message.timestamp, message.is_read);

    txn.commit();
    databaseManager_.release_connection(conn);
    return true;

  } catch (const std::exception& e) {
    std::cerr << "[MessageRepository] save failed: " << e.what() << std::endl;
    return false;
  }
}

bool MessageRepository::remove(const std::string& uuid) {
  try {
    auto conn = databaseManager_.acquire_connection();
    pqxx::work txn(*conn);

    auto result =
        txn.exec_params("DELETE FROM " + config_.table_messages() + " WHERE id = $1", uuid);

    txn.commit();
    databaseManager_.release_connection(conn);

    return result.affected_rows() > 0;

  } catch (const std::exception& e) {
    std::cerr << "[MessageRepository] remove failed: " << e.what() << std::endl;
    return false;
  }
}

Message MessageRepository::row_to_message(const pqxx::row& row) {
  Message msg;
  msg.id = row["id"].as<std::string>();
  msg.sender_id = row["sender_id"].as<std::string>();
  msg.receiver_id = row["receiver_id"].as<std::string>();
  msg.chatType = row["chat_type"].as<std::string>();
  msg.content = row["content"].as<std::string>();
  msg.timestamp = row["timestamp"].as<std::string>();
  msg.is_read = row["is_read"].as<bool>();
  return msg;
}
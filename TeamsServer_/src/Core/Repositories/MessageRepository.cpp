#include "MessageRepository.h"
#include <iostream>
#include "../../Infrastructure/Database/QueryBuilder.h"
#include "Models/Message.h"

bool MessageRepository::save(const Message& message) {
  try {
    auto conn = databaseManager_.acquire_connection();
    pqxx::work txn(*conn);

    QueryBuilder qb;

    auto query =
        qb.insert_into(config_.table_messages(), {"id", "sender_id", "receiver_id", "chat_type",
                                                  "content", "timestamp"})
            .values({"$1", "$2", "$3", "$4", "$5", "$6"})
            .build();

    txn.exec_params(query, message.id, message.sender_id, message.receiver_id, message.chatType,
                    message.content, message.timestamp);

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



std::optional<std::vector<Message>> MessageRepository::findConversationsByUserUuid(const std::string& userUuid) {
  try {
    auto conn = databaseManager_.acquire_connection();
    pqxx::work txn(*conn);

    auto result = txn.exec_params(
        "SELECT id, sender_id, receiver_id, chat_type, content, timestamp, is_read "
        "FROM " + config_.table_messages() + " "
        "WHERE sender_id = $1 OR receiver_id = $1 "
        "ORDER BY timestamp ASC",
        userUuid);

    txn.commit();
    databaseManager_.release_connection(conn);

    std::vector<Message> messages;
    messages.reserve(result.size());
    for (const auto& row : result) {
      messages.push_back(message_from_db_row(row));
    }

    return messages;

  } catch (const std::exception& e) {
    std::cerr << "[MessageRepository] findByUserUuid failed: " << e.what() << std::endl;
    return std::nullopt;
  }
}
#ifndef MESSAGEREPOSITORY_H
#define MESSAGEREPOSITORY_H

#include "../../Infrastructure/Database/DatabaseManager.h"
#include "../../Infrastructure/Database/QueryBuilder.h"
#include "../../Utils/Config.h"
#include "../../includes.h"
#include "../Models/Message.h"

class MessageRepository {
public:
  MessageRepository()
      : databaseManager_(DatabaseManager::instance()),
        config_(Config::instance()) {}

  virtual std::optional<Message> find_by_uuid(const std::string &uuid) {};
  virtual std::vector<Message> find_by_conversation(const std::string &conversation_uuid) {};

  virtual bool create(const Message &message) {};
  virtual bool delete_message(const std::string &uuid) {};

  virtual ~MessageRepository() = default;

private:
  Message row_to_message(const pqxx::row &row) {};

  Config &config_;
  DatabaseManager &databaseManager_;
};

#endif
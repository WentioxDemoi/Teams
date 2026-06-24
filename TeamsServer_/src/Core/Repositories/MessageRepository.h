#ifndef MESSAGEREPOSITORY_H
#define MESSAGEREPOSITORY_H

#include "../../Infrastructure/Database/DatabaseManager.h"

#include "../../includes.h"
#include "../Models/Message.h"

class MessageRepository {
public:
  MessageRepository()
      : databaseManager_(DatabaseManager::instance()),
        config_(Config::instance()) {}


  virtual bool save(const Message &message);
  virtual bool remove(const std::string &uuid);
  virtual std::optional<std::vector<Message>> findConversationsByUserUuid(const std::string& userUuid);

  virtual ~MessageRepository() = default;

private:

  Config &config_;
  DatabaseManager &databaseManager_;
};

#endif
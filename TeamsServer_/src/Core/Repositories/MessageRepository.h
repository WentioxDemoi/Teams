#ifndef MESSAGEREPOSITORY_H
#define MESSAGEREPOSITORY_H

#include "../../Infrastructure/Database/DatabaseManager.h"
#include "../../Utils/Config.h"
#include "../../includes.h"
#include "../Models/Message.h"

class MessageRepository {
public:
  MessageRepository()
      : databaseManager_(DatabaseManager::instance()),
        config_(Config::instance()) {}


  virtual bool save(const Message &message);
  virtual bool remove(const std::string &uuid);

  virtual ~MessageRepository() = default;

private:
  Message row_to_message(const pqxx::row &row);

  Config &config_;
  DatabaseManager &databaseManager_;
};

#endif
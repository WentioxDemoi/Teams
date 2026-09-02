#ifndef MESSAGEREPOSITORY_H
#define MESSAGEREPOSITORY_H

#include "../../Infrastructure/DatabaseManager.h"
#include "../../includes.h"
#include "../Models/Message.h"
#include "IMessageRepository.h"

class MessageRepository : public IMessageRepository {
 public:
  MessageRepository()
      : databaseManager_(DatabaseManager::instance()), config_(Config::instance()) {}

  bool save(const Message& message) override;
  bool remove(const std::string& uuid) override;
  std::optional<std::vector<Message>> findConversationsByUserUuid(
      const std::string& userUuid) override;

  ~MessageRepository() override = default;

 private:
  Config& config_;
  DatabaseManager& databaseManager_;
};

#endif
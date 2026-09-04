#ifndef IMESSAGE_REPOSITORY_H
#define IMESSAGE_REPOSITORY_H

#include <optional>
#include <string>
#include <vector>

#include "../Models/Message.h"

class IMessageRepository {
 public:
  virtual ~IMessageRepository() = default;

  virtual bool save(const Message& message) = 0;
  virtual bool remove(const std::string& uuid) = 0;
  virtual std::optional<std::vector<Message>> findConversationsByUserUuid(
      const std::string& userUuid) = 0;
};

#endif
#ifndef IMESSAGESERVICE_H
#define IMESSAGESERVICE_H

#include <optional>
#include <string>

/**
 * @class IMessageService
 * @brief Contrat métier pour la gestion des messages.
 */
class IMessageService {
 public:
  virtual ~IMessageService() = default;

  virtual std::optional<std::string> sendMessage(const std::string& payload) = 0;
  virtual std::optional<std::string> loadConversations(const std::string& userUuid) = 0;
};

#endif

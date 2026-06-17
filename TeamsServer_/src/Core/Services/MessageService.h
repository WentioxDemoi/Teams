#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "../Repositories/MessageRepository.h"
#include "../../Core/Registeries/MessageSessionRegistry.h"
#include "../Models/Message.h"
#include <memory>

/**
 * @class MessageService
 * @brief Service de gestion des messages.
 *
 * Fournit des fonctionnalités pour envoyer et charger les messages. S'appuie sur
 * MessageRepository pour accéder aux données de message et Config pour les paramètres
 * de configuration liés aux messages.
 */
class MessageService {
public:
  MessageService(std::unique_ptr<MessageRepository> messageRepo, std::shared_ptr<MessageSessionRegistry> messageSessionRegistry)
      : messageRepo_(std::move(messageRepo)), messageSessionRegistry_(messageSessionRegistry), config_(Config::instance()) {};
  virtual ~MessageService() = default;

  virtual std::optional<std::string> sendMessage(const std::string& payload);
  // virtual std::optional<std::vector<Message>> loadMessages(const std::string &token,
  //                                                          const std::string &userUuid) {};

private:
  std::unique_ptr<MessageRepository> messageRepo_;
  std::shared_ptr<MessageSessionRegistry> messageSessionRegistry_;
  Config &config_;
};

#endif
#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "../../Core/Registeries/MessageSessionRegistry.h"
#include "IMessageService.h"
#include "../Models/Message.h"
#include "../Repositories/MessageRepository.h"
#include <memory>

/**
 * @class MessageService
 * @brief Service de gestion des messages.
 *
 * Fournit des fonctionnalités pour envoyer et charger les messages. S'appuie sur
 * MessageRepository pour accéder aux données de message et Config pour les paramètres
 * de configuration liés aux messages.
 */
class MessageService : public IMessageService {
 public:
  MessageService(std::unique_ptr<MessageRepository> messageRepo,
                 std::shared_ptr<MessageSessionRegistry> messageSessionRegistry)
      : messageRepo_(std::move(messageRepo)), messageSessionRegistry_(messageSessionRegistry),
        config_(Config::instance()) {};
  ~MessageService() override = default;

  std::optional<std::string> sendMessage(const std::string &payload) override;
  std::optional<std::string> loadConversations(const std::string &userUuid) override;

 private:
  std::unique_ptr<MessageRepository> messageRepo_;
  std::shared_ptr<MessageSessionRegistry> messageSessionRegistry_;
  Config &config_;
};

#endif
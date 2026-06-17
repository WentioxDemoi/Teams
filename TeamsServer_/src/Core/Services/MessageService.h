#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "../../Utils/Crypto.h"
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

  virtual std::optional<Message> sendMessage(
                                            const std::string &senderUuid,
                                            const std::string &receiverUuid,
                                            const std::string &content) {std::cout << "Sending message..." << std::endl; }; // A modifier car c'est dans cette fonction qu'on va dépacketer le message
  virtual std::optional<std::vector<Message>> loadMessages(const std::string &token,
                                                           const std::string &userUuid) {};

private:
  std::unique_ptr<MessageRepository> messageRepo_;
  std::shared_ptr<MessageSessionRegistry> messageSessionRegistry_;
  Config &config_;
};

#endif
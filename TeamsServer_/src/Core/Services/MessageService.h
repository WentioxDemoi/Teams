#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "../../Utils/Crypto.h"
#include "../Repositories/MessageRepository.h"
#include "../Models/Message.h"

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
  MessageService(std::unique_ptr<MessageRepository> messageRepo)
      : messageRepo_(std::move(messageRepo)), config_(Config::instance()) {};
  virtual ~MessageService() = default;

  virtual std::optional<Message> sendMessage(const std::string &token,
                                            const std::string &senderUuid,
                                            const std::string &receiverUuid,
                                            const std::string &content); // A modifier car c'est dans cette fonction qu'on va dépacketer le message
  virtual std::optional<std::vector<Message>> loadMessages(const std::string &token,
                                                           const std::string &userUuid);

private:
  std::unique_ptr<MessageRepository> messageRepo_;
  Config &config_;
};

#endif
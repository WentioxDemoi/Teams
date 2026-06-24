#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "../Core/Services/MessageService.h"

using ResponseCallback = std::function<void(std::string)>;

/**
 * @class MessageHandler
 * @brief Gestionnaire des requêtes de messagerie côté serveur.
 *
 * Traite les requêtes d'envoi et de récupération de messages reçues des
 * clients, utilise MessageService pour la logique métier et renvoie les
 * réponses via un callback.
 */
class MessageHandler {
 public:
  MessageHandler(std::unique_ptr<MessageService> messageService)
      : worker_pool_(Config::instance().worker_pool_size()),
        messageService_(std::move(messageService)) {};
  ~MessageHandler() = default;

  void handle_type(std::string uuid, std::string payload, ResponseCallback respond);

 private:
  void handle_send_message(std::string uuid, std::string payload, ResponseCallback respond);
  void handle_load_conversations(std::string uuid, std::string payload, ResponseCallback respond);

  asio::thread_pool worker_pool_;
  std::unique_ptr<MessageService> messageService_;
};

#endif
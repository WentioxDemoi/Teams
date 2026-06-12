#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "../Core/Services/MessageService.h"
#include "../Utils/ResponseFormater.h"
#include "../includes.h"
#include "HandlerTools.h"
#include "../Core/Repositories/MessageRepository.h"

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
  MessageHandler(ResponseCallback respond) : worker_pool_(Config::instance().worker_pool_size()) {
    messageService_ =
        std::make_unique<MessageService>(std::make_unique<MessageRepository>());
  };
  ~MessageHandler() = default;

  void handle_type(std::string payload, ResponseCallback respond);

 private:
  void handle_send_message(std::string payload, ResponseCallback respond);
  void handle_load_messages(std::string payload, ResponseCallback respond);

  asio::thread_pool worker_pool_;
  std::unique_ptr<MessageService> messageService_;
};

#endif
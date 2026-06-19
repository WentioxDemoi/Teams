#ifndef CONTACTHANDLER_H
#define CONTACTHANDLER_H

#include "../Core/Services/ContactService.h"

using ResponseCallback = std::function<void(std::string)>;

/**
 * @class ContactHandler
 * @brief Gestionnaire des requêtes de contacts côté serveur.
 *
 * Traite les requêtes d'ajout, suppression et récupération des contacts
 * reçues des clients, utilise ContactService pour la logique métier
 * et renvoie les réponses via un callback.
 */
class ContactHandler {
 public:
  ContactHandler(std::unique_ptr<ContactService> contactService)
      : worker_pool_(Config::instance().worker_pool_size()),
        contactService_(std::move(contactService)) {}

  ~ContactHandler() = default;

  void handle_type(std::string uuid,
                   std::string payload,
                   ResponseCallback respond);

 private:
  void handle_add_contact(std::string uuid,
                          std::string payload,
                          ResponseCallback respond);

  void handle_remove_contact(std::string uuid,
                             std::string payload,
                             ResponseCallback respond);

  void handle_load_contacts(std::string uuid,
                            std::string payload,
                            ResponseCallback respond);

 private:
  asio::thread_pool worker_pool_;
  std::unique_ptr<ContactService> contactService_;
};

#endif
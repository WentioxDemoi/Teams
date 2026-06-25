#ifndef CONTACTSSION_H
#define CONTACTSESSION_H

#include <functional>
#include <memory>

#include "../../Core/Registeries/ContactSessionRegistry.h"
#include "../../Handlers/ContactHandler.h"
#include "../../Utils/BoostErrorHandler.h"
#include "../../includes.h"
#include "../../Core/Services/AuthService.h"


/**
 * @class ContactSession
 * @brief Représente une session SSL individuelle pour un client TCP.
 *
 * Gère la communication avec un client spécifique sur le port 8082,
 * et délègue le traitement des requêtes à ContactHandler.
 */
class ContactSession : public std::enable_shared_from_this<ContactSession> {
 public:
  ContactSession(tcp::socket socket, ssl::context& ctx,
                 std::shared_ptr<ContactHandler> contactHandler,
                 std::shared_ptr<ContactSessionRegistry> contactSessionRegistry,
                 std::shared_ptr<AuthService> authService)
      : stream_(std::move(socket), ctx),
        contactHandler_(contactHandler),
        contactSessionRegistry_(contactSessionRegistry),
        authService_(authService) {}

  void start();
  void send(const std::string& payload);

 private:
  void do_read();

  ssl::stream<tcp::socket> stream_;
  std::array<char, 4096> buffer_;
  std::shared_ptr<ContactHandler> contactHandler_;
  std::shared_ptr<ContactSessionRegistry> contactSessionRegistry_;
  std::shared_ptr<AuthService> authService_;
  std::string user_uuid_;
  bool isFirstMessage_ = true;
};

#endif
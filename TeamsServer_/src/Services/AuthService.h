#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../Repositories/AuthRepository.h"
#include "../Structs/NatsReplyContext.h"
#include "../includes.h"
#include "NatsClient.h"

class AuthService {
public:
  AuthService(asio::thread_pool &dbPool) : db_pool(dbPool) {
    NatsClient::instance().subscribe("auth.register",
                                     &AuthService::onRegisterMsg, this);
    NatsClient::instance().subscribe("auth.login", &AuthService::onLoginMsg,
                                     this);
    NatsClient::instance().subscribe("auth.validate_token", &AuthService::onValidateTokenMsg,
                                     this);
    // Créer nouvelle route pour validation token pour mon service messages et
    // faire en sorte que l'appel depuis service message soit bloquant
    repository = &AuthRepository::instance();
  }

private:
  void handle_register(const std::string &payload,
                       const NatsReplyContext &replyCtx);
  void handle_login(const std::string &payload,
                    const NatsReplyContext &replyCtx);
  void handle_validate_token(const std::string &payload,
                    const NatsReplyContext &replyCtx);

  void sendReply(const NatsReplyContext &ctx, const std::string &payload);

  // ======================================================
  // CallBacks
  // ======================================================
  static void onRegisterMsg(natsConnection *, natsSubscription *, natsMsg *msg,
                            void *closure);
  static void onLoginMsg(natsConnection *, natsSubscription *, natsMsg *msg,
                         void *closure);
  static void onValidateTokenMsg(natsConnection *, natsSubscription *, natsMsg *msg,
                         void *closure);

  asio::thread_pool &db_pool;
  AuthRepository *repository;
};
#endif
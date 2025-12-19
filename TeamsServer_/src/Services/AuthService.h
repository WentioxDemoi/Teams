#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../includes.h"
#include "NatsClient.h"
#include "../Database/Database.h"
#include "../Structs/NatsReplyContext.h"


class AuthService {
public:
  AuthService(asio::thread_pool &dbPool) : db_pool(dbPool) {
    NatsClient::instance().subscribe("auth.register",
                                     &AuthService::onRegisterMsg, this);
    database = &Database::instance();
  }

private:
  void handle_register(const std::string& payload,
    const NatsReplyContext& replyCtx);

  void sendReply(const NatsReplyContext& ctx, const std::string& payload);

  // ======================================================
  // CallBacks
  // ======================================================
  static void onRegisterMsg(natsConnection *, natsSubscription *, natsMsg *msg,
                            void *closure);

  asio::thread_pool &db_pool;
  Database *database;
};
#endif
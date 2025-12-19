#include "AuthService.h"

void AuthService::handle_register(const std::string &payload,
                                  const NatsReplyContext &replyCtx) {
  // Parsing
  // DB
  // Validation

  std::string result = "...";

  sendReply(replyCtx, result);
}

void AuthService::sendReply(const NatsReplyContext &ctx, const std::string &payload) {
  if (ctx.reply_to.empty())
    return;

  natsConnection_Publish(ctx.nc, ctx.reply_to.c_str(), payload.data(),
                         payload.size());
}

// ======================================================
// CallBacks
// ======================================================

void AuthService::onRegisterMsg(natsConnection *nc, natsSubscription *,
                                natsMsg *msg, void *closure) {
  auto *self = static_cast<AuthService *>(closure);

  std::string payload(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));

  NatsReplyContext replyCtx{nc,
                            natsMsg_GetReply(msg) ? natsMsg_GetReply(msg) : ""};

  asio::post(self->db_pool, [self, payload, replyCtx, msg]() {
    self->handle_register(payload, replyCtx);
    natsMsg_Destroy(msg);
  });

  
}
#include "AuthService.h"
#include "../Sessions/SessionTools.h"

void AuthService::handle_register(const std::string &payload,
                                  const NatsReplyContext &replyCtx) {
    auto username = SessionTools::extractValue(payload, "username");
    auto password = SessionTools::extractValue(payload, "password");

    auto response = database->authenticate(username, password, true);

    std::string result;
    if (!response.has_value()) {
        result = R"({"error":"Registration failed: username may already exist or invalid data."})";
    } else {
        result = "{"
                 "\"data\":{"
                 "\"id\":" + std::to_string(response->id) + "," +
                 "\"username\":\"" + response->username + "\"," +
                 "\"token\":\"" + response->token + "\"," +
                 "\"status\":\"" + response->status + "\"" +
                 "}" 
                 "}";
    }

    sendReply(replyCtx, result);
}

void AuthService::handle_login(const std::string &payload,
                               const NatsReplyContext &replyCtx) {
    auto username = SessionTools::extractValue(payload, "username");
    auto password = SessionTools::extractValue(payload, "password");

    auto response = database->authenticate(username, password, false);

    std::string result;
    if (!response.has_value()) {
        result = R"({"error":"Login failed: invalid username or password."})";
    } else {
        result = "{"
                 "\"data\":{"
                 "\"id\":" + std::to_string(response->id) + "," +
                 "\"username\":\"" + response->username + "\"," +
                 "\"token\":\"" + response->token + "\"," +
                 "\"status\":\"" + response->status + "\"" +
                 "}" 
                 "}";
    }

    sendReply(replyCtx, result);
}

void AuthService::sendReply(const NatsReplyContext &ctx,
                            const std::string &payload) {
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
                                  std::cout << "aaaa" << std::endl;
  std::string payload(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));
  std::cout << "Payload : " << payload << std::endl;

  NatsReplyContext replyCtx{nc,
                            natsMsg_GetReply(msg) ? natsMsg_GetReply(msg) : ""};

  asio::post(self->db_pool, [self, payload, replyCtx, msg]() {
    self->handle_register(payload, replyCtx);
    natsMsg_Destroy(msg);
  });
}

void AuthService::onLoginMsg(natsConnection *nc , natsSubscription *, natsMsg *msg, void *closure)
{
  auto *self = static_cast<AuthService *>(closure);

  std::string payload(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));

  NatsReplyContext replyCtx{nc,
                            natsMsg_GetReply(msg) ? natsMsg_GetReply(msg) : ""};

  asio::post(self->db_pool, [self, payload, replyCtx, msg]() {
    self->handle_login(payload, replyCtx);
    natsMsg_Destroy(msg);
  });
}
#include "AuthService.h"
#include "../Sessions/SessionTools.h"
#include "../includes.h"
#include "ServicesTools.h"

void AuthService::handle_register(const std::string &payload,
                                  const NatsReplyContext &replyCtx)
{
    auto email    = SessionTools::extractValue(payload, "email");
    auto username = SessionTools::extractValue(payload, "username");
    auto password = SessionTools::extractValue(payload, "password");

    auto response = repository->authenticate(email, password, true, username);

    std::string result;

    if (!response.has_value())
    {
        result = R"({"error":"Registration failed: username may already exist or invalid data."})";
    }
    else
    {
        result =
            "{"
                "\"data\":{"
                    "\"id\":" + std::to_string(response->id) + ","
                    "\"username\":\"" + ServicesTools::jsonEscape(response->username) + "\","
                    "\"email\":\"" + ServicesTools::jsonEscape(response->email) + "\","
                    "\"token\":\"" + ServicesTools::jsonEscape(response->token) + "\","
                    "\"status\":\"" + ServicesTools::jsonEscape(response->status) + "\""
                "}"
            "}";
    }

    sendReply(replyCtx, result);
}

void AuthService::handle_login(const std::string &payload,
                               const NatsReplyContext &replyCtx)
{
    auto email    = SessionTools::extractValue(payload, "email");
    auto password = SessionTools::extractValue(payload, "password");

    auto response = repository->authenticate(email, password, false);

    std::string result;

    if (!response.has_value())
    {
        result = R"({"error":"Login failed: invalid username or password."})";
    }
    else
    {
        result =
            "{"
                "\"data\":{"
                    "\"id\":" + std::to_string(response->id) + ","
                    "\"username\":\"" + ServicesTools::jsonEscape(response->username) + "\","
                    "\"email\":\"" + ServicesTools::jsonEscape(response->email) + "\","
                    "\"token\":\"" + ServicesTools::jsonEscape(response->token) + "\","
                    "\"status\":\"" + ServicesTools::jsonEscape(response->status) + "\""
                "}"
            "}";
    }

    sendReply(replyCtx, result);
}

// ======================================================
// NATS Reply
// ======================================================

void AuthService::sendReply(const NatsReplyContext &ctx,
                            const std::string &payload)
{
    if (ctx.reply_to.empty())
        return;

    natsConnection_Publish(
        ctx.nc,
        ctx.reply_to.c_str(),
        payload.data(),
        payload.size()
    );
}

// ======================================================
// Callbacks
// ======================================================

void AuthService::onRegisterMsg(natsConnection *nc,
                                natsSubscription *,
                                natsMsg *msg,
                                void *closure)
{
    auto *self = static_cast<AuthService *>(closure);

    std::string payload(
        natsMsg_GetData(msg),
        natsMsg_GetDataLength(msg)
    );

    NatsReplyContext replyCtx{
        nc,
        natsMsg_GetReply(msg) ? natsMsg_GetReply(msg) : ""
    };

    asio::post(self->db_pool, [self, payload, replyCtx, msg]()
    {
        self->handle_register(payload, replyCtx);
        natsMsg_Destroy(msg);
    });
}

void AuthService::onLoginMsg(natsConnection *nc,
                             natsSubscription *,
                             natsMsg *msg,
                             void *closure)
{
    auto *self = static_cast<AuthService *>(closure);

    std::string payload(
        natsMsg_GetData(msg),
        natsMsg_GetDataLength(msg)
    );

    NatsReplyContext replyCtx{
        nc,
        natsMsg_GetReply(msg) ? natsMsg_GetReply(msg) : ""
    };

    asio::post(self->db_pool, [self, payload, replyCtx, msg]()
    {
        self->handle_login(payload, replyCtx);
        natsMsg_Destroy(msg);
    });
}
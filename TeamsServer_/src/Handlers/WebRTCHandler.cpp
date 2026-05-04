#include "WebRTCHandler.h"
#include "../Network/Session/WebRTCSession.h"

void WebRTCHandler::handle_type(std::string payload,
                               ResponseCallback respond,
                               std::shared_ptr<WebRTCSession> session) {
    std::cout << "[WebRTCHandler] handle_type called\n";

    std::string type = HandlerTools::extractValue(payload, "type");
    std::cout << "[WebRTCHandler] Message type: '" << type << "'\n";

    if (type.empty()) {
        std::cerr << "[WebRTCHandler] Empty message type\n";
        return;
    }

    if (type == "register") {
        std::cout << "[WebRTCHandler] Dispatching to handle_register\n";
        handle_register(payload, respond, session);
    } else if (type == "offer" || type == "answer" || type == "ice") {
        std::cout << "[WebRTCHandler] Dispatching to handle_send\n";
        handle_send(payload, respond);
    } else {
        std::cerr << "[WebRTCHandler] Unknown message type: " << type << "\n";
    }
}

void WebRTCHandler::handle_register(std::string payload,
                                   ResponseCallback respond,
                                   std::shared_ptr<WebRTCSession> session) {
    std::cout << "[WebRTCHandler] handle_register called\n";

    auto token = HandlerTools::extractValue(payload, "token");
    std::cout << "[WebRTCHandler] Extracted token: "
              << (token.empty() ? "<empty>" : "<present>") << "\n";

    asio::post(worker_pool_, [this, token, respond, session]() {
        std::cout << "[WebRTCHandler][Worker] Processing register request\n";

        try {
            auto user = WebRTCService_->find_by_token(token);

            if (!user.has_value()) {
                std::cerr << "[WebRTCHandler][Worker] Invalid token\n";
                respond(R"({"type":"register_response","error":"Registration failed: invalid token."})");
                return;
            }

            std::cout << "[WebRTCHandler][Worker] User authenticated, uuid="
                      << user->uuid << "\n";

            session->set_uuid(user->uuid);
            registry_->register_session(user->uuid, session);

            std::cout << "[WebRTCHandler][Worker] Session registered\n";
            respond(R"({"type":"register_response","status":"ok"})");

        } catch (const std::exception& e) {
            std::cerr << "[WebRTCHandler][Worker] Register exception: "
                      << e.what() << "\n";
            respond(R"({"type":"register_response","error":"Registration failed: server error"})");
        }
    });
}

void WebRTCHandler::handle_send(std::string payload, ResponseCallback respond) {
    auto target_uuid = HandlerTools::extractValue(payload, "target_uuid");

    asio::post(worker_pool_, [this, target_uuid, payload, respond]() {
        if (target_uuid.empty()) {
            respond(R"({"type":"send_response","error":"Missing target_uuid"})");
            return;
        }
        auto target = registry_->find(target_uuid);
        if (!target) {
            respond(R"({"type":"send_response","error":"User not connected"})");
            return;
        }
        // On forward le payload brut tel quel au destinataire
        asio::post(target->stream_.get_executor(), [target, payload]() {
            target->send(payload);
        });
        respond(R"({"type":"send_response","status":"ok"})");
    });
}
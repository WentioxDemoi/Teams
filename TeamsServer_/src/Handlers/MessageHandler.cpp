#include "MessageHandler.h"

void MessageHandler::handle_send_message(std::string payload, ResponseCallback respond) {
  auto token         = HandlerTools::extractValue(payload, "token");
  auto senderUuid    = HandlerTools::extractValue(payload, "senderUuid");
  auto receiverUuid  = HandlerTools::extractValue(payload, "receiverUuid");
  auto content       = HandlerTools::extractValue(payload, "content");

  asio::post(worker_pool_, [this, token, senderUuid, receiverUuid, content, respond]() {
    try {
      auto response = messageService_->sendMessage(token, senderUuid, receiverUuid, content);
      std::string result;
      if (!response.has_value()) {
        result = R"({"type":"message_sent","error":"Send failed: invalid data or unauthorized."})";
      } else {
        result = ResponseFormater::format_message_response("message_sent", *response);
      }
      respond(result);
    } catch (const std::exception& e) {
      std::cerr << "[MessageHandler] Send error: " << e.what() << "\n";
      respond(R"({"type":"message_sent","error":"Send failed: server error"})");
    }
  });
}

void MessageHandler::handle_load_messages(std::string payload, ResponseCallback respond) {
  auto token    = HandlerTools::extractValue(payload, "token");
  auto userUuid = HandlerTools::extractValue(payload, "userUuid");

  asio::post(worker_pool_, [this, token, userUuid, respond]() {
    try {
      auto response = messageService_->loadMessages(token, userUuid);
      std::string result;
      if (!response.has_value()) {
        result = R"({"type":"conversation_response","error":"Load failed: invalid token or user."})";
      } else {
        // result = ResponseFormater::format_messages_response("conversation_response", userUuid, *response);
      }
      respond(result);
    } catch (const std::exception& e) {
      std::cerr << "[MessageHandler] Load error: " << e.what() << "\n";
      respond(R"({"type":"conversation_response","error":"Load failed: server error"})");
    }
  });
}

void MessageHandler::handle_type(std::string payload, ResponseCallback respond) {
  std::string type = HandlerTools::extractValue(payload, "type");
  std::cout << "type : " << type << std::endl;
  if (type.empty())
    return;

  if (type == "send_message") {
    handle_send_message(payload, respond);
  } else if (type == "load_messages") {
    handle_load_messages(payload, respond);
  } else {
    std::cerr << "[MessageHandler] Unknown message type: " << type << "\n";
  }
}
#include "MessageHandler.h"
#include "../Utils/PacketHelper.h"
#include <optional>

void MessageHandler::handle_send_message(std::string uuid, std::string payload, ResponseCallback respond) {

  PacketHelper::insertValue(payload, "senderUuid", uuid);
  std::cout << "[MessageHandler] Sending message with payload: " << payload << std::endl;
  asio::post(worker_pool_, [this, payload, respond]() {
    try {
      std::optional<std::string> response = messageService_->sendMessage(payload);
      if (!response.has_value()) {
        response = R"({"type":"message_sent","error":"Send failed: invalid data or unauthorized."})";
      }
      respond(response.value());
    } catch (const std::exception& e) {
      std::cerr << "[MessageHandler] Send error: " << e.what() << "\n";
      respond(R"({"type":"message_sent","error":"Send failed: server error"})");
    }
  });
}

void MessageHandler::handle_load_messages(std::string uuid, std::string payload, ResponseCallback respond) {
  // auto token    = HandlerTools::extractValue(payload, "token");
  // auto userUuid = HandlerTools::extractValue(payload, "userUuid");

  // asio::post(worker_pool_, [this, token, userUuid, respond]() {
  //   try {
  //     auto response = messageService_->loadMessages(token, userUuid);
  //     std::string result;
  //     if (!response.has_value()) {
  //       result = R"({"type":"conversation_response","error":"Load failed: invalid token or user."})";
  //     } else {
  //       // result = ResponseFormater::format_messages_response("conversation_response", userUuid, *response);
  //     }
  //     respond(result);
  //   } catch (const std::exception& e) {
  //     std::cerr << "[MessageHandler] Load error: " << e.what() << "\n";
  //     respond(R"({"type":"conversation_response","error":"Load failed: server error"})");
  //   }
  // });
}

void MessageHandler::handle_type(std::string uuid, std::string payload, ResponseCallback respond) {
  std::string type = PacketHelper::extractValue(payload, "type");
  std::cout << "type : " << type << std::endl;
  if (type.empty())
    return;

  if (type == "send_message") {
    handle_send_message(uuid, payload, respond);
  } else if (type == "load_messages") {
    handle_load_messages(uuid, payload, respond);
  } else {
    std::cerr << "[MessageHandler] Unknown message type: " << type << "\n";
  }
}
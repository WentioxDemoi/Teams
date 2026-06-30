#include "WebRTCHandler.h"
#include "../Network/Session/WebRTCSession.h"
#include "../Utils/PacketHelper.h"

void WebRTCHandler::handle_type(std::string uuid, std::string payload, ResponseCallback respond) {
  std::string type = PacketHelper::extractValue(payload, "type");
  std::cout << "[WebRTCHandler] Message type: '" << type << "'\n";

  if (type.empty()) {
    std::cerr << "[WebRTCHandler] Empty message type\n";
    return;
  }

  if (type == "call_request") {
    handle_call_request(uuid, payload, respond);
  } else if (type == "call_accept") {
    handle_call_accept(uuid, payload, respond);
  } else if (type == "call_reject") {
    handle_call_reject(uuid, payload, respond);
  } else if (type == "call_cancel") {
    handle_call_cancel(uuid, payload, respond);
  } else if (type == "call_hangup") {
    handle_call_hangup(uuid, payload, respond);
  } else if (type == "offer" || type == "answer" || type == "ice") {
    handle_signaling_relay(uuid, payload, respond);
  } else {
    std::cerr << "[WebRTCHandler] Unknown message type: " << type << "\n";
  }
}

void WebRTCHandler::handle_call_request(const std::string &uuid, std::string payload, ResponseCallback respond) {
  std::string targetUuid = PacketHelper::extractValue(payload, "targetUuid");

  asio::post(worker_pool_, [this, uuid, targetUuid, respond]() {
    try {
      auto response = webRTCService_->requestCall(uuid, targetUuid);
      if (response.has_value()) {
        respond(response.value());
      }
    } catch (const std::exception &e) {
      std::cerr << "[WebRTCHandler] call_request error: " << e.what() << "\n";
      respond(R"({"type":"error","error":"call_request failed"})");
    }
  });
}

void WebRTCHandler::handle_call_accept(const std::string &uuid, std::string payload, ResponseCallback respond) {
  std::string targetUuid = PacketHelper::extractValue(payload, "targetUuid");

  asio::post(worker_pool_, [this, uuid, targetUuid]() {
    webRTCService_->acceptCall(uuid, targetUuid);
  });
}

void WebRTCHandler::handle_call_reject(const std::string &uuid, std::string payload, ResponseCallback respond) {
  std::string targetUuid = PacketHelper::extractValue(payload, "targetUuid");

  asio::post(worker_pool_, [this, uuid, targetUuid]() {
    if (!webRTCService_->rejectCall(uuid, targetUuid)) {
      std::cerr << "[WebRTCHandler] call_reject: caller " << targetUuid << " déconnecté\n";
    }
  });
}

void WebRTCHandler::handle_call_cancel(const std::string &uuid, std::string payload, ResponseCallback respond) {
  std::string targetUuid = PacketHelper::extractValue(payload, "targetUuid");

  asio::post(worker_pool_, [this, uuid, targetUuid]() {
    if (!webRTCService_->cancelCall(uuid, targetUuid)) {
      std::cerr << "[WebRTCHandler] call_cancel: callee " << targetUuid << " déconnecté\n";
    }
  });
}

void WebRTCHandler::handle_call_hangup(const std::string &uuid, std::string payload, ResponseCallback respond) {
  std::string targetUuid = PacketHelper::extractValue(payload, "targetUuid");

  asio::post(worker_pool_, [this, uuid, targetUuid]() {
    if (!webRTCService_->hangupCall(uuid, targetUuid)) {
      std::cerr << "[WebRTCHandler] call_hangup: pair " << targetUuid << " déjà déconnecté\n";
    }
  });
}

void WebRTCHandler::handle_signaling_relay(const std::string &uuid, std::string payload, ResponseCallback respond) {
  std::string targetUuid = PacketHelper::extractValue(payload, "targetUuid");

  asio::post(worker_pool_, [this, uuid, targetUuid, payload]() {
    if (!webRTCService_->relaySignaling(uuid, targetUuid, payload)) {
      std::cerr << "[WebRTCHandler] signaling relay: target " << targetUuid << " déconnecté\n";
    }
  });
}
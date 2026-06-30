#include "WebRTCService.h"
#include "../../Utils/PacketHelper.h"

bool WebRTCService::relayToTarget(const std::string &senderUuid, const std::string &targetUuid,
                                   std::string payload) {
  PacketHelper::insertValue(payload, "senderUuid", senderUuid);
  return webRTCRegistry_->sendMessage(targetUuid, payload);
}

std::optional<std::string> WebRTCService::requestCall(const std::string &callerUuid, const std::string &targetUuid) {
  if (!webRTCRegistry_->isConnected(targetUuid)) {
    std::cout << "[WebRTCService] requestCall: target offline (" << targetUuid << ")\n";
    return R"({"type":"callee_offline"})";
  }

  // TODO: si une notion de "busy" (callee déjà en appel) est trackée côté serveur,
  // vérifier ici et retourner {"type":"busy"} le cas échéant.

  return R"({"type":"call_request_ack"})";
}

void WebRTCService::acceptCall(const std::string &calleeUuid, const std::string &targetUuid) {
  std::cout << "[WebRTCService] acceptCall: " << calleeUuid << " accepte l'appel de " << targetUuid << "\n";
  // TODO: mettre à jour un éventuel état serveur "in_call" pour calleeUuid et targetUuid.
}

bool WebRTCService::rejectCall(const std::string &calleeUuid, const std::string &targetUuid) {
  return relayToTarget(calleeUuid, targetUuid, R"({"type":"call_reject"})");
}

bool WebRTCService::cancelCall(const std::string &callerUuid, const std::string &targetUuid) {
  return relayToTarget(callerUuid, targetUuid, R"({"type":"call_cancel"})");
}

bool WebRTCService::hangupCall(const std::string &senderUuid, const std::string &targetUuid) {
  return relayToTarget(senderUuid, targetUuid, R"({"type":"call_hangup"})");
}

bool WebRTCService::relaySignaling(const std::string &senderUuid, const std::string &targetUuid,
                                    const std::string &payload) {
  return relayToTarget(senderUuid, targetUuid, payload);
}
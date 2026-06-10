#include "CallService.h"

CallService::CallService(NetworkService* network, QObject* parent)
    : ICallService(parent),
      network_(network ? network : new NetworkService(8083, parent)) {
  Q_ASSERT(network_);
  connect(network_, &NetworkService::jsonReceived, this, &CallService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this, &CallService::callError);
  connect(network_, &NetworkService::connectionUpdate, this, &CallService::connectionUpdate);
}

void CallService::startCall(const QString& calleeUuid) {
  if (calleeUuid.trimmed().isEmpty()) {
    emit callError("UUID du destinataire invalide");
    return;
  }

  network_->send({{"type", "call_start"}, {"calleeUuid", calleeUuid}});
}

void CallService::acceptCall(const QString& callUuid) {
  if (callUuid.trimmed().isEmpty()) {
    emit callError("UUID d'appel invalide");
    return;
  }

  network_->send({{"type", "call_accept"}, {"callUuid", callUuid}});
}

void CallService::hangup(const QString& callUuid) {
  if (callUuid.trimmed().isEmpty()) {
    emit callError("UUID d'appel invalide");
    return;
  }

  network_->send({{"type", "call_hangup"}, {"callUuid", callUuid}});
}

void CallService::disconnectFromServer() {
  network_->disconnectFromServer();
}

void CallService::handleServerResponse(const QJsonObject& root) {
  if (!root.contains("type") || !root["type"].isString()) {
    emit callError("Réponse serveur manquante ou invalide");
    return;
  }

  const QString type = root["type"].toString();
  if (root.contains("error") && root["error"].isString()) {
    emit callError(root["error"].toString());
    return;
  }

  if (type == "call_started" && root.contains("callUuid") && root.contains("calleeUuid")) {
    emit callStarted(root["callUuid"].toString(), root["calleeUuid"].toString());
    return;
  }

  if (type == "call_accepted" && root.contains("callUuid") && root.contains("callerUuid")) {
    emit callAccepted(root["callUuid"].toString(), root["callerUuid"].toString());
    return;
  }

  if (type == "call_ended" && root.contains("callUuid") && root.contains("reason")) {
    emit callEnded(root["callUuid"].toString(), root["reason"].toString());
    return;
  }
}

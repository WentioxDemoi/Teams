#include "CallService.h"
#include "Core/State/SessionState.h"
#include "P2P/WebRTCService.h"
#include "ServiceLocator.h"

#include <QQmlEngine>

CallService::CallService(NetworkService *network, WebRTCService *webRTCService, QObject *parent)
    : ICallService(parent), network_(network ? network : new NetworkService(8083, parent)),
      webRTCService_(webRTCService ? webRTCService : ServiceLocator::instance().getService<WebRTCService>()) {
  Q_ASSERT(network_);
  Q_ASSERT(webRTCService_);

  connect(network_, &NetworkService::jsonReceived, this, &CallService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this, &CallService::callError);
  connect(network_, &NetworkService::connectionUpdate, &SessionState::instance(),
          &SessionState::onServerConnectionUpdate);

  connect(this, &ICallService::offerReceived, webRTCService_, &WebRTCService::onRemoteOffer);
  connect(this, &ICallService::answerReceived, webRTCService_, &WebRTCService::onRemoteAnswer);
  connect(this, &ICallService::iceReceived, webRTCService_, &WebRTCService::onRemoteIce);

  connect(this, &ICallService::triggerCreateOffer, webRTCService_, &WebRTCService::startCall);

  std::function<void(const std::string &sdp)> onLocalOffer = [this](const std::string &sdp) {
    QJsonObject data;
    data["sdp"] = QString::fromStdString(sdp);
    sendSignalingMessage(WebRTCMessageType::offer, data);
  };

  std::function<void(const std::string &sdp)> onLocalAnswer = [this](const std::string &sdp) {
    QJsonObject data;
    data["sdp"] = QString::fromStdString(sdp);
    sendSignalingMessage(WebRTCMessageType::answer, data);
  };

  std::function<void(const std::string &candidate, const std::string &mid, int index)> onLocalIce =
      [this](const std::string &candidate, const std::string &mid, int index) {
        QJsonObject data;
        data["candidate"] = QString::fromStdString(candidate);
        data["mid"] = QString::fromStdString(mid);
        data["index"] = index;
        sendSignalingMessage(WebRTCMessageType::ice, data);
      };

  std::function<void(bool inProgress)> isContactConnectedChanged = [this](bool inProgress) {
    // Ici on pourra emit un signal pour notifier l'UI que le contact est connecté ou rencontre des problèmes de co.
    // WebRTC déclenche ce callback lorsque l'état de connection change
  };

  webRTCService_->setCallBacks(onLocalOffer, onLocalAnswer, onLocalIce, isContactConnectedChanged);
}

// Caller : l'utilisateur initie un appel vers un contact, on envoie un signal au serveur pour vérifier que le contact
// est bien co.
void CallService::startCall(const QString &contactUuid, const QString &contactUsername) {
  if (inCall_) {
    qDebug() << "[CallService] Un appel est déjà en cours.\n";
    emit callError("Un appel est déjà en cours");
    return;
  }

  inCall_ = true;
  remoteUuid_ = contactUuid;
  remoteUsername_ = contactUsername;

  emit openCallWindow(contactUsername);

  // Ici on vérifie depuis le serveur que le contact est bien co (même si on fait déjà une vérification côté ViewModel)
  // Si l'utilisateur est bien co au serveur, le serveur nous renvoie un "ack" qui déclenche triggerCreateOffer() pour
  // lancer la négociation WebRTC.
  QJsonObject payload;
  payload["type"] = "call_request";
  payload["targetUuid"] = contactUuid;
  network_->send(payload);

  startCallTimeoutTimer();
}

// Callee : l'utilisateur reçoit un appel entrant, il refuse et on envoie un signal au serveur pour notifier le caller.
void CallService::rejectCall() {
  qDebug() << "[CallService] Appel refusé. Notification du caller :" << remoteUuid_ << "\n";

  QJsonObject payload;
  payload["type"] = "call_reject";
  payload["targetUuid"] = remoteUuid_;
  network_->send(payload);

  inCall_ = false;
  remoteUuid_.clear();
  pendingOfferSdp_.clear();

  // Pas de fenêtre à fermer ici: côté callee, la fenêtre d'appel ne s'ouvre
  // qu'à l'acceptation (voir acceptCall), donc rien n'a encore été créé.
}

// Callee : l'utilisateur reçoit un appel entrant, il accepte et on déclenche la négociation WebRTC
void CallService::acceptCall(const QString &remoteUsername) {
  qDebug() << "AcceptCall\n";
  if (pendingOfferSdp_.isEmpty()) {
    qWarning() << "acceptCall appelé sans offer en attente.";
    emit callError("Aucun appel entrant à accepter");
    return;
  }

  emit openCallWindow(remoteUsername);

  // On déclenche la suite de la négociation WebRTC seulement maintenant:
  // WebRTCService::onRemoteOffer va appeler SetRemoteDescription -> CreateAnswer -> SetLocalDescription
  // ce qui re-déclenchera onLocalAnswer -> sendSignalingMessage(answer) automatiquement.
  emit offerReceived(pendingOfferSdp_);
  emit isContactConnectedChanged(true);
  pendingOfferSdp_.clear();

  QJsonObject payload;
  payload["type"] = "call_accept";
  payload["targetUuid"] = remoteUuid_;
  network_->send(payload);
}

// Callee || Caller : l'utilisateur raccroche, on notifie l'autre participant et on ferme la fenêtre d'appel.
void CallService::hangup() {
  qDebug() << "[CallService] HangUp, envoie du message à : " << remoteUuid_ << "\n";
  if (inCall_) {
    if (!remoteUuid_.isEmpty()) {
      QJsonObject payload;
      payload["type"] = "call_hangup";
      payload["targetUuid"] = remoteUuid_;
      network_->send(payload);
    }

    inCall_ = false;
    remoteUuid_.clear();
    pendingOfferSdp_.clear();

    emit closeCallWindow();
  }
}

// Caller : Démarre le timer de timeout pour l'appel, si le callee ne répond pas dans les 30s, on annule l'appel.
void CallService::startCallTimeoutTimer() {
  // Sécurité: si un timer existait déjà (ne devrait pas arriver vu la garde inCall_
  // dans startCall, mais on protège contre une fuite si jamais réutilisé).
  if (callTimeoutTimer_) {
    callTimeoutTimer_->stop();
    callTimeoutTimer_->deleteLater();
    callTimeoutTimer_ = nullptr;
  }

  callTimeoutTimer_ = new QTimer(this);
  callTimeoutTimer_->setSingleShot(true);
  connect(callTimeoutTimer_, &QTimer::timeout, this, [this]() {
    qWarning() << "[CallService] Timeout: pas de réponse du callee.";

    QJsonObject payload;
    payload["type"] = "call_cancel";
    payload["targetUuid"] = remoteUuid_;
    network_->send(payload);

    inCall_ = false;
    remoteUuid_.clear();

    // La fenêtre du caller doit se fermer automatiquement si personne ne répond.
    emit closeCallWindow();
    // emit callError("Pas de réponse");
  });
  callTimeoutTimer_->start(30000);
}

// Caller || Callee : on notifie l'autre qu'on active/désactive notre caméra, pour que l'UI de l'autre participant
// puisse s'adapter.
void CallService::cameraEnabledChanged(bool cameraEnabled) {
  QJsonObject payload;
  payload["type"] = "camera_enabled_change";
  payload["value"] = cameraEnabled ? "true" : "false";
  payload["targetUuid"] = remoteUuid_;
  network_->send(payload);
}

void CallService::handleServerResponse(const QJsonObject &root) {
  qDebug() << "[CallService] RAW RECEIVED:" << root;
  if (!root.contains("type") || !root["type"].isString())
    return;

  const QString type = root["type"].toString();
  const QString senderUuid = root.value("senderUuid").toString();

  if (type == "offer" && root.contains("sdp") && root["sdp"].isString()) {
    if (inCall_) {
      qWarning() << "Offer reçue alors qu'un appel est déjà en cours, rejet automatique.";

      QJsonObject payload;
      payload["type"] = "busy";
      payload["targetUuid"] = senderUuid;
      network_->send(payload);
      return;
    }
    remoteUuid_ = senderUuid;
    inCall_ = true;
    pendingOfferSdp_ = root["sdp"].toString();

    emit incomingCallReceived(senderUuid);

  } else if (type == "answer" && root.contains("sdp") && root["sdp"].isString()) {
    if (callTimeoutTimer_) {
      callTimeoutTimer_->stop();
    }
    emit answerReceived(root["sdp"].toString());

  } else if (type == "ice" && root.contains("candidate") && root.contains("mid") && root.contains("index")) {
    emit iceReceived(root["candidate"].toString(), root["mid"].toString(), root["index"].toInt());
  } else if (type == "call_request_ack") {
    emit triggerCreateOffer();
  } else if (type == "callee_offline") {
    if (callTimeoutTimer_) {
      callTimeoutTimer_->stop();
    }
    inCall_ = false;
    remoteUuid_.clear();
    emit closeCallWindow();
    // emit callError("Le contact est hors ligne");

  } else if (type == "call_reject") {
    if (callTimeoutTimer_) {
      callTimeoutTimer_->stop();
    }
    inCall_ = false;
    remoteUuid_.clear();
    emit closeCallWindow();
    // emit callError("Appel refusé");

  } else if (type == "call_cancel") {
    inCall_ = false;
    remoteUuid_.clear();
    pendingOfferSdp_.clear();
    emit incomingCallCancelled(senderUuid);

  } else if (type == "call_accept") {
    emit isContactConnectedChanged(true);
  } else if (type == "call_hangup") {
    inCall_ = false;
    remoteUuid_.clear();
    emit closeCallWindow();
    // emit callError("L'autre participant a raccroché");

  } else if (type == "busy") {
    if (callTimeoutTimer_) {
      callTimeoutTimer_->stop();
    }
    inCall_ = false;
    remoteUuid_.clear();
    emit closeCallWindow();
    // emit callError("Le contact est déjà en appel");
  } else if (type == "camera_enabled_change" && root.contains("value") && root["value"].isString()) {
    bool isEnabled = root["value"].toString() == "true";
    emit onCameraEnabledChanged(isEnabled);
  }
}

void CallService::sendSignalingMessage(WebRTCMessageType type, const QJsonObject &data) {
  QMetaObject::invokeMethod(
      this,
      [this, type, data]() {
        QJsonObject payload;
        payload["targetUuid"] = remoteUuid_;

        switch (type) {
        case WebRTCMessageType::offer:
          qDebug() << "[WebRTCService] sending offer";
          payload["type"] = "offer";
          payload["sdp"] = data["sdp"];
          break;

        case WebRTCMessageType::answer:
          qDebug() << "[WebRTCService] sending answer";
          payload["type"] = "answer";
          payload["sdp"] = data["sdp"];
          break;

        case WebRTCMessageType::ice:
          payload["type"] = "ice";
          payload["candidate"] = data["candidate"];
          payload["mid"] = data["mid"];
          payload["index"] = data["index"];
          break;
        }

        network_->send(payload);
      },
      Qt::QueuedConnection);
}

void CallService::disconnectFromServer() { network_->disconnectFromServer(); }
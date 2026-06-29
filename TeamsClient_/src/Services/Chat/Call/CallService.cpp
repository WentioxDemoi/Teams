#include "CallService.h"
#include "P2P/WebRTCService.h"
#include "ServiceLocator.h"
#include <QtQml/qqmlengine.h>

CallService::CallService(NetworkService *network, WebRTCService *webRTCService, QObject *parent)
    : ICallService(parent), network_(network ? network : new NetworkService(8083, parent)),
      webRTCService_(webRTCService ? webRTCService : ServiceLocator::instance().getService<WebRTCService>()) {
  Q_ASSERT(network_);
  Q_ASSERT(webRTCService_);

  connect(network_, &NetworkService::jsonReceived, this, &CallService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this, &CallService::callError);
  connect(network_, &NetworkService::connectionUpdate, this, &CallService::connectionUpdate);

  connect(this, &ICallService::offerReceived, webRTCService_, &WebRTCService::onRemoteOffer);
  connect(this, &ICallService::answerReceived, webRTCService_, &WebRTCService::onRemoteAnswer);
  connect(this, &ICallService::iceReceived, webRTCService_, &WebRTCService::onRemoteIce);

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

  std::function<void(bool inProgress)> onP2PChange = [this](bool inProgress) {
    QMetaObject::invokeMethod(
        this,
        [this, inProgress]() {
          qDebug() << "[WebRTCService] onP2PChange connected=" << inProgress;
          emit this->onP2PChange(inProgress);
        },
        Qt::QueuedConnection);
  };

  webRTCService_->setCallBacks(onLocalOffer, onLocalAnswer, onLocalIce, onP2PChange);
}

// === CALLER : démarre un appel sortant ===
void CallService::startCall(const QString &contactUuid, const QString &contactUsername) {
  if (inCall_) {
    qWarning() << "Un appel est déjà en cours.";
    emit callError("Un appel est déjà en cours");
    return;
  }

  inCall_ = true;
  remoteUuid_ = contactUuid;
  remoteUsername_ = contactUsername;

  // Ouverture de la fenêtre d'appel sortant (état "ça sonne...") déléguée à
  // WebRTCViewModel via signal ; CallService n'attend pas de confirmation.
  emit openCallWindow(contactUsername);

  // On notifie le serveur qu'on souhaite appeler ce contact.
  // Le serveur doit vérifier la présence du callee avant de relayer quoi que ce soit;
  // tant qu'on n'a pas reçu "call_request_ack", on ne déclenche PAS encore CreateOffer().
  network_->send({{"type", "call_request"}, {"targetUuid", contactUuid}});

  startCallTimeoutTimer();
}

// === CALLEE : popup affiché, l'utilisateur refuse ===
void CallService::rejectCall() {
  network_->send({{"type", "call_reject"}, {"targetUuid", remoteUuid_}});
  inCall_ = false;
  remoteUuid_.clear();
  pendingOfferSdp_.clear();
  // Pas de fenêtre à fermer ici: côté callee, la fenêtre d'appel ne s'ouvre
  // qu'à l'acceptation (voir acceptCall), donc rien n'a encore été créé.
}

// === CALLEE : popup affiché, l'utilisateur accepte ===
void CallService::acceptCall() {

  if (pendingOfferSdp_.isEmpty()) {
    qWarning() << "acceptCall appelé sans offer en attente.";
    emit callError("Aucun appel entrant à accepter");
    return;
  }

  // Ouverture de la fenêtre d'appel côté callee, au moment de l'acceptation
  // (et non à la réception de l'offer, pour ne pas afficher une fenêtre tant
  // que l'utilisateur n'a pas explicitement décroché). Délégué à WebRTCViewModel.
  emit openCallWindow(remoteUsername_);

  // On déclenche la suite de la négociation WebRTC seulement maintenant:
  // WebRTCService::onRemoteOffer va appeler SetRemoteDescription -> CreateAnswer -> SetLocalDescription
  // ce qui re-déclenchera onLocalAnswer -> sendSignalingMessage(answer) automatiquement.
  emit offerReceived(pendingOfferSdp_);
  pendingOfferSdp_.clear();

  network_->send({{"type", "call_accept"}, {"targetUuid", remoteUuid_}});
}

// === Raccroche un appel en cours (caller ou callee, après acceptation) ===
void CallService::hangup() {

  if (!remoteUuid_.isEmpty()) {
    network_->send({{"type", "call_hangup"}, {"targetUuid", remoteUuid_}});
  }

  if (callTimeoutTimer_) {
    callTimeoutTimer_->stop();
  }

  inCall_ = false;
  remoteUuid_.clear();
  pendingOfferSdp_.clear();

  // Fermeture de la fenêtre d'appel si elle existe encore (gérée par WebRTCViewModel).
  emit closeCallWindow();
}

void CallService::handleServerResponse(const QJsonObject &root) {
  qDebug() << "[SignalingClient] RAW RECEIVED:" << root;
  if (!root.contains("type") || !root["type"].isString())
    return;

  const QString type = root["type"].toString();
  const QString senderUuid = root.value("senderUuid").toString();

  if (type == "offer" && root.contains("sdp") && root["sdp"].isString()) {
    // === CALLEE reçoit une offer ===
    if (inCall_) {
      qWarning() << "Offer reçue alors qu'un appel est déjà en cours, rejet automatique.";
      network_->send({{"type", "busy"}, {"targetUuid", senderUuid}});
      return;
    }
    remoteUuid_ = senderUuid;
    inCall_ = true;
    pendingOfferSdp_ = root["sdp"].toString();
    // On affiche le popup d'appel entrant ; on n'émet PAS offerReceived ici.
    // C'est acceptCall() qui le fera, seulement si l'utilisateur accepte.
    emit incomingCallReceived(senderUuid);

  } else if (type == "answer" && root.contains("sdp") && root["sdp"].isString()) {
    // === CALLER reçoit l'answer : le callee a accepté et négocié son SDP ===
    if (callTimeoutTimer_) {
      callTimeoutTimer_->stop();
    }
    emit answerReceived(root["sdp"].toString());

  } else if (type == "ice" && root.contains("candidate") && root.contains("mid") && root.contains("index")) {
    emit iceReceived(root["candidate"].toString(), root["mid"].toString(), root["index"].toInt());

  } else if (type == "call_request_ack") {
    // === CALLER : le serveur confirme que le callee est joignable ===
    // C'est seulement maintenant qu'on déclenche la création de l'offer WebRTC.
    emit triggerCreateOffer();

  } else if (type == "callee_offline") {
    // === CALLER : le callee n'est pas connecté, on annule tout de suite ===
    if (callTimeoutTimer_) {
      callTimeoutTimer_->stop();
    }
    inCall_ = false;
    remoteUuid_.clear();
    emit closeCallWindow();
    emit callError("Le contact est hors ligne");

  } else if (type == "call_reject") {
    // === CALLER : le callee a explicitement refusé ===
    if (callTimeoutTimer_) {
      callTimeoutTimer_->stop();
    }
    inCall_ = false;
    remoteUuid_.clear();
    emit closeCallWindow();
    emit callError("Appel refusé");

  } else if (type == "call_cancel") {
    // === CALLEE : le caller a annulé avant qu'on réponde (ou son propre timeout a expiré) ===
    inCall_ = false;
    remoteUuid_.clear();
    pendingOfferSdp_.clear();
    emit incomingCallCancelled(senderUuid);

  } else if (type == "call_hangup") {
    // === L'autre pair raccroche un appel en cours ===
    inCall_ = false;
    remoteUuid_.clear();
    emit closeCallWindow();
    emit callError("L'autre participant a raccroché");

  } else if (type == "busy") {
    // === CALLER : le callee est déjà en appel ailleurs ===
    if (callTimeoutTimer_) {
      callTimeoutTimer_->stop();
    }
    inCall_ = false;
    remoteUuid_.clear();
    emit closeCallWindow();
    emit callError("Le contact est déjà en appel");
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

// === CALLER : démarre le timer "pas de réponse" ===
// Couvre le cas où le callee ne répond ni par accept ni par reject avant expiration.
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
    network_->send({{"type", "call_cancel"}, {"targetUuid", remoteUuid_}});

    inCall_ = false;
    remoteUuid_.clear();

    // La fenêtre du caller doit se fermer automatiquement si personne ne répond.
    emit closeCallWindow();

    emit callError("Pas de réponse");
  });
  callTimeoutTimer_->start(30000); // 30s, à ajuster
}

void CallService::disconnectFromServer() { network_->disconnectFromServer(); }
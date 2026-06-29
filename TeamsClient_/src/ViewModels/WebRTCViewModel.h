#ifndef WEBRTCVIEWMODEL_H
#define WEBRTCVIEWMODEL_H

#include "../Services/P2P/WebRTCService.h"
#include "ViewModelsTools.h"
#include <QQmlComponent>
#include <QQuickWindow>
#include <QVideoSink>
#include <QtQml/qqmlcontext.h>
#include <QtQml/qqmlengine.h>

/**
 * @class WebRTCViewModel
 * @brief ViewModel pour la gestion WebRTC.
 *
 * Coordonne l'initialisation et le suivi des connexions P2P via WebRTC,
 * en relayant les changements d'état et en gérant l'enregistrement auprès
 * du serveur pour l'identifiant unique de l'utilisateur.
 *
 * Gère également l'ouverture de la fenêtre d'appel audio/vidéo (CallWindow.qml) :
 * les QVideoSink ne sont plus possédés par ce ViewModel (VideoOutput.videoSink est
 * en lecture seule en Qt6, c'est VideoOutput qui crée son propre sink). Le QML nous
 * transmet ses sinks via setLocalVideoSink/setRemoteVideoSink ; on se contente de
 * les retenir pour pouvoir y pousser les frames WebRTC reçues.
 */

class WebRTCViewModel : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool cameraEnabled READ cameraEnabled NOTIFY cameraEnabledChanged)
  Q_PROPERTY(bool micEnabled READ micEnabled NOTIFY micEnabledChanged)
  Q_PROPERTY(QString remoteUsername READ remoteUsername NOTIFY remoteUsernameChanged)

public:
  explicit WebRTCViewModel(QQmlEngine *engine, WebRTCService *webRTCService = nullptr, QObject *parent = nullptr);

  void start(); // OLD

  bool cameraEnabled() const { return cameraEnabled_; }
  bool micEnabled() const { return micEnabled_; }
  QString remoteUsername() const { return remoteUsername_; }

public slots:
  void initP2P(); // OLD


  void setLocalVideoSink(QVideoSink *sink) { localVideoSink_ = sink; }
  void setRemoteVideoSink(QVideoSink *sink) { remoteVideoSink_ = sink; }

  void toggleCamera() {
    cameraEnabled_ = !cameraEnabled_;
    // TODO: activer/désactiver réellement la capture caméra / le track WebRTC correspondant.
    emit cameraEnabledChanged();
  }

  void toggleMic() {
    micEnabled_ = !micEnabled_;
    // TODO: idem pour la piste audio.
    emit micEnabledChanged();
  }

  void endCall() {
    if (!callWindow_) return;  // déjà en cours de fermeture / déjà fermé

    // TODO: fermer la PeerConnection, notifier le pair distant via signaling.
    QQuickWindow *window = callWindow_;
    callWindow_ = nullptr;  // évite un double deleteLater() si endCall() est appelé deux fois
                              // (ex: clic sur le bouton ✕ QML puis fermeture native de la fenêtre)

    window->close();
    window->deleteLater();  // close() masque seulement la fenêtre ; deleteLater()
                              // détruit réellement l'objet (et donc le QQmlContext
                              // parenté, libérant ses ressources avant la fermeture
                              // de l'application).
    emit callEnded();
  }

signals:
  void onP2PChange(bool inProgress);
  void registerWithServer4WebRTC(QString UUID); // OLD
  void cameraEnabledChanged();
  void micEnabledChanged();
  void remoteUsernameChanged();
  void callEnded();

private:
  QQmlEngine *engine_;
  WebRTCService *webRTCService_;

  QQuickWindow *callWindow_ = nullptr;
  QVideoSink *localVideoSink_ = nullptr;
  QVideoSink *remoteVideoSink_ = nullptr;

  bool cameraEnabled_ = false;
  bool micEnabled_ = false;
  QString remoteUsername_;
};

#endif
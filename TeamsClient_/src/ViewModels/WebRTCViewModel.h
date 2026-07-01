#ifndef WEBRTCVIEWMODEL_H
#define WEBRTCVIEWMODEL_H

#include "../Services/P2P/WebRTCService.h"
#include "Chat/ChatService.h"
#include "ViewModelsTools.h"
#include <QCamera>
#include <QMediaCaptureSession>
#include <QObject>
#include <QQmlComponent>
#include <QPointer>
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
  explicit WebRTCViewModel(QQmlEngine *engine, WebRTCService *webRTCService = nullptr,
                           IChatService *chatService = nullptr, QObject *parent = nullptr);

  Q_INVOKABLE bool cameraEnabled() const { return cameraEnabled_; }
  Q_INVOKABLE bool micEnabled() const { return micEnabled_; }
  Q_INVOKABLE QString remoteUsername() const { return remoteUsername_; }

public slots:

  void setLocalVideoSink(QVideoSink *sink);
  void setRemoteVideoSink(QVideoSink *sink) { remoteVideoSink_ = sink; }

  void toggleCamera() {
    if (!cameraEnabled_) {
      startCamera();
    } else {
      stopCamera();
    }
    cameraEnabled_ = !cameraEnabled_;

    emit cameraEnabledChanged();
  }

  void toggleMic() {
    micEnabled_ = !micEnabled_;
    // TODO: idem pour la piste audio.
    emit micEnabledChanged();
  }

  void acceptIncomingCall();
  void rejectIncomingCall();
  void hangup();

  void onOpenCallWindow(const QString &remoteUsername);
  void onCloseCallWindow();

private:
  void startCamera();
  void stopCamera();

signals:
  void onP2PChange(bool inProgress);
  void onCallEnded();
  void cameraEnabledChanged();
  void micEnabledChanged();
  void remoteUsernameChanged();

private:
  QQmlEngine *engine_;
  WebRTCService *webRTCService_;
  IChatService *chatService_;
  QCamera *camera_ = nullptr;
  QVideoSink *captureSink_ = nullptr;
  QMediaCaptureSession captureSession_;

  QQuickWindow *callWindow_ = nullptr;
  QPointer<QVideoSink> localVideoSink_ = nullptr;
  QVideoSink *remoteVideoSink_ = nullptr;

  bool cameraEnabled_ = false;
  bool micEnabled_ = false;
  QString remoteUsername_;
};

#endif
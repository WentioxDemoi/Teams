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
  Q_PROPERTY(bool isContactConnected READ isContactConnected NOTIFY isContactConnectedChanged)
  Q_PROPERTY(bool isRemoteCameraEnabled READ isRemoteCameraEnabled NOTIFY isRemoteCameraEnabledChanged)

public:
  explicit WebRTCViewModel(QQmlEngine *engine, WebRTCService *webRTCService = nullptr,
                           IChatService *chatService = nullptr, QObject *parent = nullptr);

  Q_INVOKABLE bool cameraEnabled() const { return cameraEnabled_; }
  Q_INVOKABLE bool micEnabled() const { return micEnabled_; }
  Q_INVOKABLE QString remoteUsername() const { return remoteUsername_; }
  Q_INVOKABLE bool isContactConnected() const { return isContactConnected_; }
  Q_INVOKABLE bool isRemoteCameraEnabled() const { return isRemoteCameraEnabled_; }

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

    chatService_->cameraEnabledChanged(cameraEnabled_);
    emit cameraEnabledChanged();
  }

  void toggleMic() {
    micEnabled_ = !micEnabled_;
    // TODO: idem pour la piste audio.
    emit micEnabledChanged();
  }


  void hangup();

  void onOpenCallWindow(const QString &remoteUsername);
  void onCloseCallWindow();

private:
  void startCamera();
  void stopCamera();
  void onIsContactConnectedChanged(bool isConnected) {
    qDebug() << "Is contact connected changed:" << isConnected;
    isContactConnected_ = isConnected;
    emit isContactConnectedChanged();
  }
  void onRemoteCameraEnabledChanged(bool cameraEnabled) {
    isRemoteCameraEnabled_ = cameraEnabled;
    if (!cameraEnabled && remoteVideoSink_) {
    // Aucune nouvelle frame n'arrivera tant que la caméra distante est coupée :
    // sans ça, QVideoSink continue d'afficher la dernière frame reçue indéfiniment.
    remoteVideoSink_->setVideoFrame(QVideoFrame());
  }
    emit isRemoteCameraEnabledChanged();
  }

signals:
  void onCallEnded();
  void cameraEnabledChanged();
  void micEnabledChanged();
  void remoteUsernameChanged();
  void isRemoteCameraEnabledChanged();
  void isContactConnectedChanged();

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
  bool isContactConnected_ = false;
  bool isRemoteCameraEnabled_ = false;
};

#endif
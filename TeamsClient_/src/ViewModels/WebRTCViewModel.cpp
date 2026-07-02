#include "WebRTCViewModel.h"

#include "../Core/ServiceLocator.h"
#include "Interfaces/IChatService.h"
#include <QtQml/qqmlengine.h>
#include "../Services/P2P/Sources.h"

WebRTCViewModel::WebRTCViewModel(QQmlEngine *engine, WebRTCService *webRTCService, IChatService *chatService, QObject *parent)
    : webRTCService_(webRTCService ? webRTCService : ServiceLocator::instance().getService<WebRTCService>()),
      chatService_(chatService ? chatService : ServiceLocator::instance().getService<IChatService>()),
      engine_(engine), QObject(parent) {
    connect(chatService_, &IChatService::openCallWindow, this, &WebRTCViewModel::onOpenCallWindow);
    connect(chatService_, &IChatService::closeCallWindow, this, &WebRTCViewModel::onCloseCallWindow);
    connect(chatService_, &IChatService::isContactConnectedChanged, this, &WebRTCViewModel::onIsContactConnectedChanged);
    captureSink_ = new QVideoSink(this);
    connect(captureSink_, &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame& frame) {
        if (!frame.isValid()) {
            qWarning() << "[WebRTCViewModel] captureSink_ frame invalide";
            return;
        }
        if (localVideoSink_) {
            localVideoSink_->setVideoFrame(frame);
        }

        auto i420 = FrameConverter::NV12ToI420(frame);
        if (!i420) {
            qWarning() << "[WebRTCViewModel] Impossible de convertir la frame locale NV12 -> I420";
            return;
        }
        Sources::instance().localVideo()->PushFrame(i420);
    });

    connect(chatService_, &IChatService::onCameraEnabledChanged, this, &WebRTCViewModel::onRemoteCameraEnabledChanged);
}

void WebRTCViewModel::onOpenCallWindow(const QString &remoteUsername) {
  if (callWindow_) {
    qWarning() << "[WebRTCViewModel] Une fenêtre d'appel est déjà ouverte, ignoré.";
    return;
  }

  remoteUsername_ = remoteUsername;
  emit remoteUsernameChanged();

  QQmlComponent component(engine_);
  component.loadFromModule("TeamsClient", "CallWindow");

  QObject *windowObj = component.create(engine_->rootContext());
  if (!windowObj) {
    qWarning() << "[WebRTCViewModel] Échec de création de CallWindow.qml:" << component.errorString();
    return;
  }

  callWindow_ = qobject_cast<QQuickWindow *>(windowObj);
  if (!callWindow_) {
    qWarning() << "[WebRTCViewModel] L'objet créé n'est pas une QQuickWindow.";
    windowObj->deleteLater();
    return;
  }

  Sources::instance().remoteVideo()->setFrameCallback(
        [this](QVideoFrame frame) {
            // OnFrame est appelé depuis un thread libwebrtc → marshalling Qt obligatoire
            QMetaObject::invokeMethod(this, [this, frame]() {
                if (remoteVideoSink_)
                    remoteVideoSink_->setVideoFrame(frame);
            }, Qt::QueuedConnection);
        }
    );

  // Le ViewModel reste propriétaire du cycle de vie de la fenêtre.
  QQmlEngine::setObjectOwnership(callWindow_, QQmlEngine::CppOwnership);

  // Si l'utilisateur ferme la fenêtre via le bouton système (croix),
  // on veut nettoyer l'état proprement plutôt que de garder un pointeur mort.
  connect(callWindow_, &QQuickWindow::visibleChanged, this, [this](bool visible) {
    if (!visible) {
      hangup();
    }
  });

  callWindow_->show();
}

void WebRTCViewModel::onCloseCallWindow() {
  if (!callWindow_) {
    return;
  }
  Sources::instance().remoteVideo()->setFrameCallback(nullptr);
  // hide() plutôt que close() pour éviter une réentrance dans le destructeur
  // si ce slot est appelé depuis le signal visibleChanged ci-dessus.
  callWindow_->hide();
  callWindow_->deleteLater();
  callWindow_ = nullptr;

  localVideoSink_ = nullptr;
  remoteVideoSink_ = nullptr;
  
  cameraEnabled_ = false;
  micEnabled_ = false;

  isContactConnected_ = false;
  isRemoteCameraEnabled_ = false;
  cameraEnabledChanged();
  micEnabledChanged();
  isRemoteCameraEnabledChanged();
  isContactConnectedChanged();

  remoteUsername_.clear();
  stopCamera();
  emit remoteUsernameChanged();
}

void WebRTCViewModel::hangup() {
  if (chatService_) {
    chatService_->hangup();
  }
}

void WebRTCViewModel::startCamera() {
    camera_ = new QCamera(this);
    captureSession_.setCamera(camera_);
    if (captureSink_) {
        captureSession_.setVideoSink(captureSink_);
    }
    camera_->start();
}

void WebRTCViewModel::setLocalVideoSink(QVideoSink *sink) {
    localVideoSink_ = sink;
    if (!captureSink_) {
        qWarning() << "[WebRTCViewModel] setLocalVideoSink appelé sans captureSink_ existant";
        return;
    }

    if (camera_) {
        captureSession_.setVideoSink(captureSink_);
    }

}

void WebRTCViewModel::stopCamera() {

    captureSession_.setCamera(nullptr);
    captureSession_.setVideoSink(nullptr);

    if (camera_) {
        camera_->stop();
        delete camera_;
        camera_ = nullptr;
    }
}
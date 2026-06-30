#include "WebRTCViewModel.h"

#include "../Core/ServiceLocator.h"
#include "Interfaces/IChatService.h"
#include <QtQml/qqmlengine.h>

WebRTCViewModel::WebRTCViewModel(QQmlEngine *engine, WebRTCService *webRTCService, IChatService *chatService, QObject *parent)
    : webRTCService_(webRTCService ? webRTCService : ServiceLocator::instance().getService<WebRTCService>()),
    chatService_(chatService ? chatService : ServiceLocator::instance().getService<IChatService>()),
      engine_(engine), QObject(parent) {
  start();
}

void WebRTCViewModel::start() {
  // connect(webRTCService_, &WebRTCService::onP2PChange, this, &WebRTCViewModel::onP2PChange);
  // connect(this, &WebRTCViewModel::registerWithServer4WebRTC, webRTCService_,
  //         &WebRTCService::registerWithServer4WebRTC);
  connect(chatService_, &IChatService::openCallWindow, this, &WebRTCViewModel::onOpenCallWindow);
  connect(chatService_, &IChatService::closeCallWindow, this, &WebRTCViewModel::onCloseCallWindow);
}

void WebRTCViewModel::initP2P() { webRTCService_->startCall(); }

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

  // Le ViewModel reste propriétaire du cycle de vie de la fenêtre.
  QQmlEngine::setObjectOwnership(callWindow_, QQmlEngine::CppOwnership);

  // Si l'utilisateur ferme la fenêtre via le bouton système (croix),
  // on veut nettoyer l'état proprement plutôt que de garder un pointeur mort.
  connect(callWindow_, &QQuickWindow::visibleChanged, this, [this](bool visible) {
    if (!visible) {
      onCloseCallWindow();
    }
  });

  callWindow_->show();
}

void WebRTCViewModel::onCloseCallWindow() {
  if (!callWindow_) {
    return;
  }

  // hide() plutôt que close() pour éviter une réentrance dans le destructeur
  // si ce slot est appelé depuis le signal visibleChanged ci-dessus.
  callWindow_->hide();
  callWindow_->deleteLater();
  callWindow_ = nullptr;

  localVideoSink_ = nullptr;
  remoteVideoSink_ = nullptr;

  remoteUsername_.clear();
  emit remoteUsernameChanged();

}
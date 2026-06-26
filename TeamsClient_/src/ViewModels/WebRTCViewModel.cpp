#include "WebRTCViewModel.h"

#include "../Core/ServiceLocator.h"
#include <QtQml/qqmlengine.h>

WebRTCViewModel::WebRTCViewModel(QQmlEngine *engine, WebRTCService *webRTCService, QObject *parent)
    : webRTCService_(webRTCService ? webRTCService : ServiceLocator::instance().getService<WebRTCService>()),
      engine_(engine), QObject(parent) {
  start();
}

void WebRTCViewModel::start() {
  // connect(webRTCService_, &WebRTCService::onP2PChange, this, &WebRTCViewModel::onP2PChange);
  // connect(this, &WebRTCViewModel::registerWithServer4WebRTC, webRTCService_,
  //         &WebRTCService::registerWithServer4WebRTC);
}

void WebRTCViewModel::initP2P() { webRTCService_->startCall(); }
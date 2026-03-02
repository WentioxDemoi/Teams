#include "WebRTCViewModel.h"

WebRTCViewModel::WebRTCViewModel(WebRTCService *webRTCService, QObject *parent) : webRTCService_(webRTCService ? webRTCService : ServiceLocator::instance().getService<WebRTCService>()), QObject(parent)
{
    start();
}

void WebRTCViewModel::start()
{
    connect(webRTCService_, &WebRTCService::onP2PChange, this, &WebRTCViewModel::onP2PChange);
    connect(this, &WebRTCViewModel::registerWithServer4WebRTC, webRTCService_, &WebRTCService::registerWithServer4WebRTC);
    
}

void WebRTCViewModel::initP2P()
{
    qDebug() << "Init P2P";
    webRTCService_->startCall();
}
#include "WebRTCViewModel.h"
#include "ServiceLocator.h"
#include "WebRTCService.h"
#include "webrtc_includes.h"
#include <QtCore/qobject.h>

WebRTCViewModel::WebRTCViewModel(WebRTCService *webRTCService, QObject *parent) : webRTCService_(webRTCService ? webRTCService : ServiceLocator::instance().getService<WebRTCService>()), QObject(parent)
{

}

void WebRTCViewModel::start()
{
    
}

void WebRTCViewModel::initP2P()
{
    qDebug() << "Init P2P";
}
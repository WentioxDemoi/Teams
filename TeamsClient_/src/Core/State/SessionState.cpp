#include "SessionState.h"
#include <QtCore/qdebug.h>

void SessionState::onServerConnectionUpdate(ServerType server, bool status)
{
    switch (server) {
        case ServerType::Auth:
            connectedToAuth = status;
            break;
        case ServerType::WebRTC:
            connectedToWebRTC = status;
            break;
        case ServerType::Message:
            connectedToMessage = status;
            break;
        default:
            qDebug() << "Unkonown enum (onServerConnectionUpdate)";
    }
    qDebug() << "Auth : " << connectedToAuth << " WebRTC : " << connectedToWebRTC << " Message : " << connectedToMessage;
}


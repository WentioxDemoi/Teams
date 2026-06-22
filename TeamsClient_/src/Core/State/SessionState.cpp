#include "SessionState.h"

#include <QtCore/qdebug.h>

SessionState& SessionState::instance() {
  static SessionState instance;
  return instance;
}

void SessionState::onServerConnectionUpdate(ServerType server, bool status) {
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
    case ServerType::Call:
      connectedToCall = status;
      break;
    case ServerType::Contact:
      connectedToContact = status;
      break;
    default:
      qDebug() << "Unkonown enum (onServerConnectionUpdate)";
  }
  qDebug() << "Auth : " << connectedToAuth << " WebRTC : " << connectedToWebRTC
           << " Message : " << connectedToMessage << " Call : " << connectedToCall << " Contact : " << connectedToContact;
}

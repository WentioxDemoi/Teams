#ifndef SESSIONSTATE_H
#define SESSIONSTATE_H

#include <QObject>
#include "SessionEnum.h"



/**
 * @class SessionState
 * @brief Singleton gérant l'état global de la session en cours.
 *
 * Suit la connectivité aux différents services (auth, WebRTC, messagerie)
 * et centralise les informations transverses de session.
 */
class SessionState : public QObject {
    Q_OBJECT

public:
  static SessionState& instance();

public slots:
    void onServerConnectionUpdate(ServerType server, bool status);

signals:
    void onApplicationQuit();



private:
    bool connectedToAuth = false;
    bool connectedToWebRTC = false;
    bool connectedToMessage = false;
    bool connectedToCall = false;
    bool connectedToContact = false;
};

#endif
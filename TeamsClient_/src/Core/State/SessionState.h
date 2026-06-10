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

// Bon ici normalement on devrait avoir des trucs comme ça :

    // bool connectedToGateway; genre les différents micros services
    // bool inCall; Avec notification automatique vers le serveur

    // QString currentChannelId; Déjà géré par QML
    // QString currentMeetingId; Déjà géré par QML

    // QVector<QString> openedChats; Déjà géré par QML
    // Notifications
    // État de sync avec les serveurs

public slots:
    void onServerConnectionUpdate(ServerType server, bool status);



private:
    bool connectedToAuth = false;
    bool connectedToWebRTC = false;
    bool connectedToMessage = false;
    bool connectedToCall = false;
};

#endif
#ifndef SESSIONSTATE_H
#define SESSIONSTATE_H

#include <QObject>

class SessionState : public QObject {
    Q_OBJECT

public:
 // Bon ici normalement on devrait avoir des trucs comme ça :

    // bool connectedToGateway; genre les différents micros services
    // bool inCall; Avec notification automatique vers le serveur

    // QString currentChannelId; Déjà géré par QML
    // QString currentMeetingId; Déjà géré par QML

    // QVector<QString> openedChats; Déjà géré par QML
    // Notifications
    // État de sync avec les serveurs


};

#endif
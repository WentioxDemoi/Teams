#ifndef AUTHNETWORKSERVICE_H
#define AUTHNETWORKSERVICE_H

#include "Interfaces/IAuthNetworkService.h"
#include "NetworkService.h"
#include <cstddef>

/**
 * @class AuthNetworkService
 * @brief Service réseau pour gérer les requêtes d'authentification.
 *
 * Envoie les requêtes login et register au serveur via QSslSocket,
 * reçoit et parse les réponses JSON, et émet les signaux authSuccess ou authError
 * selon le résultat.
 */
class AuthNetworkService : public IAuthNetworkService {
    Q_OBJECT

public:
    explicit AuthNetworkService(NetworkService *network = nullptr, QObject *parent = nullptr);

    void loginUser(const QString &email, const QString &password) override;
    void registerUser(const QString &email,
                      const QString &username,
                      const QString &password) override;
    void validateToken(const QString &value) override;
    void disconnectFromServer() override;

public slots:
    void handleServerResponse(const QJsonObject &root) override;

private:
    NetworkService *network_;
};

#endif
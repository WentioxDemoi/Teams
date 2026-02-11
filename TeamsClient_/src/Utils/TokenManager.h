#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include "Interfaces/ITokenManager.h"

/**
 * @class TokenManager
 * @brief Gère l'accès au token de l'utilisateur (Création, lecture et suppression)
 *
 * Émet des signaux selon la demande initiale.
 */
class TokenManager : public ITokenManager
{
    Q_OBJECT
public:
    static TokenManager &instance();

    

    // Méthodes synchronisées pour manipuler le token
    bool readToken() override;   // Retourne true si le token a été lu
    bool writeToken(const QString &value) override; // Retourne true si écrit
    bool deleteToken() override; // Retourne true si supprimé


private:
    explicit TokenManager(QObject *parent = nullptr);
    QKeychain::ReadPasswordJob m_readCredentialJob;
    QKeychain::WritePasswordJob m_writeCredentialJob;
    QKeychain::DeletePasswordJob m_deleteCredentialJob;
};

#endif
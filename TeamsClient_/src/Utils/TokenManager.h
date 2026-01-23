#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include "../includes.h"

/**
 * @class TokenManager
 * @brief Gère l'accès au token de l'utilisateur (Création, lecture et suppression)
 *
 * Émet des signaux selon la demande initiale.
 */
class TokenManager : public QObject
{
    Q_OBJECT
public:
    explicit TokenManager(QObject *parent = nullptr);

    // Méthodes synchronisées pour manipuler le token
    bool readToken(const QString &key);   // Retourne true si le token a été lu
    bool writeToken(const QString &key, const QString &value); // Retourne true si écrit
    bool deleteToken(const QString &key); // Retourne true si supprimé

    QString token;

private:
    QKeychain::ReadPasswordJob m_readCredentialJob;
    QKeychain::WritePasswordJob m_writeCredentialJob;
    QKeychain::DeletePasswordJob m_deleteCredentialJob;
};

#endif
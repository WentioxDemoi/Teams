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
    void readKey(const QString &key);
    void writeKey(const QString &key, const QString &value);
    void deleteKey(const QString &key);

signals:
    void keyStored(const QString &key);
    void keyRestored(const QString &key, const QString &value);
    void keyDeleted(const QString &key);
    void error(const QString &errorText);

private:
    QKeychain::ReadPasswordJob m_readCredentialJob;
    QKeychain::WritePasswordJob m_writeCredentialJob;
    QKeychain::DeletePasswordJob m_deleteCredentialJob;
};

#endif
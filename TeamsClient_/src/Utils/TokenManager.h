#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include <qt6keychain/keychain.h>
#include <qt6keychain/qkeychain_export.h>

#include <QDebug>
#include <QEventLoop>
#include <QObject>
#include <QString>

#include "Interfaces/ITokenManager.h"

/**
 * @class TokenManager
 * @brief Gère l'accès au token de l'utilisateur (Création, lecture et suppression)
 *
 * Émet des signaux selon la demande initiale.
 */
class TokenManager : public ITokenManager {
  Q_OBJECT
 public:
  static TokenManager& instance();

  // Méthodes synchronisées pour manipuler le token
  bool readToken() override;
  bool writeToken(const QString& value) override;
  bool deleteToken() override;

 private:
  explicit TokenManager(QObject* parent = nullptr);
  QKeychain::ReadPasswordJob m_readCredentialJob;
  QKeychain::WritePasswordJob m_writeCredentialJob;
  QKeychain::DeletePasswordJob m_deleteCredentialJob;
};

#endif
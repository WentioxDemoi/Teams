#ifndef ITOKENMANAGER_H
#define ITOKENMANAGER_H

#include <QObject>

/**
 * @class ITokenManager
 * @brief Interface de gestion de jetons d'authentification.
 *
 * Définit les méthodes pour lire, écrire et supprimer un jeton,
 * permettant aux différentes implémentations de gérer les informations
 * d'authentification de manière uniforme.
 */
class ITokenManager : public QObject {
  Q_OBJECT
 public:
  explicit ITokenManager(QObject* parent = nullptr) : QObject(parent) {}
  virtual ~ITokenManager() = default;
  virtual bool writeToken(const QString&) = 0;
  virtual bool deleteToken() = 0;
  virtual bool readToken() = 0;
  QString token;

};

#endif
#ifndef ISESSIONSERVICE_H
#define ISESSIONSERVICE_H

#include <QObject>
#include "User.h"

/**
 * @class ISessionService
 * @brief Interface pour la gestion des sessions utilisateur.
 *
 * Définit les méthodes pour connecter, enregistrer et initialiser une session utilisateur,
 * ainsi que les signaux pour notifier le succès, les erreurs d'authentification
 * ou l'absence de jeton.
 */
class ISessionService : public QObject {
  Q_OBJECT
 public:
  explicit ISessionService(QObject* parent = nullptr) : QObject(parent) {};
  virtual ~ISessionService() = default;

  virtual void loginUser(const QString& email, const QString& password) = 0;
  virtual void registerUser(const QString& firstName, const QString& lastName, const QString& email,
                            const QString& password) = 0;
  virtual void start() = 0;

 signals:
  void authSuccess(const User& user);
  void authError(const QString& error);
  void noTokenFound();

 public slots:
  virtual void onLocalUserSaved(const User& user) = 0;
  virtual void errorToken(const QString& error) = 0;
};

#endif
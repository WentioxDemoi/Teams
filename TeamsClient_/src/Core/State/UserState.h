#ifndef USERSTATE_H
#define USERSTATE_H

#include <QObject>

#include "User.h"

/**
 * @class UserState
 * @brief Singleton gérant l'état de l'utilisateur local.
 *
 * Stocke et expose l'utilisateur actuellement connecté.
 * Émet un signal lors de la sauvegarde pour notifier les composants abonnés.
 */
class UserState : public QObject {
  Q_OBJECT

 public:
  const User& localUser() const { return user_; }

  static UserState& instance();

  UserState() = default;
  ~UserState() = default;
  UserState(const UserState&) = delete;
  UserState(UserState&&) = delete;

  UserState& operator=(const UserState&) = delete;
  UserState& operator=(UserState&&) = delete;

 public slots:
  void saveLocalUser(const User& user);
  void updateLocalUser(const User &user) {};
  void deleteLocalUser() { user_ = User(); };

 signals:
  void localUserSaved(const User& user_);

 private:
  User user_;
};

#endif
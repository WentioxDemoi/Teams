#ifndef USERSTATE_H
#define USERSTATE_H

#include <QObject>

#include "User.h"

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
#include "UserState.h"

UserState &UserState::instance() {
  static UserState instance;
  return instance;
}

void UserState::saveLocalUser(const User& user) {
  if (user_.uuid() == user.uuid()) return;

  user_ = user;
  qDebug() << "UserState emitting localUserSaved for" << user_.uuid();
  emit localUserSaved(user_);
}
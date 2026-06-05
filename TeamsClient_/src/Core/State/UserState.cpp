#include "UserState.h"

UserState &UserState::instance() {
  static UserState instance;
  return instance;
}

void UserState::saveLocalUser(const User& user) {
  if (user_.uuid() == user.uuid()) return;

  user_ = user;
  emit localUserSaved(user_);
}
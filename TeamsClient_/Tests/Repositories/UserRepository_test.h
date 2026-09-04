#ifndef USER_REPOSITORY_TEST_H
#define USER_REPOSITORY_TEST_H

#include <QObject>

class UserRepositoryTest : public QObject {
  Q_OBJECT

 private slots:
  void saveAndFindUserRoundTrip();
  void saveUsesUuidAsUpsertKey();
  void findAllReturnsStoredUsers();
  void updateLastReadAtUpdatesOnlyExistingUser();
  void removeDeletesOnlyRequestedUser();
  void removeAllContactsKeepsLocalUser();
  void removeAllClearsUsers();
};

#endif
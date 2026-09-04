#ifndef CONTACT_LIST_TEST_H
#define CONTACT_LIST_TEST_H

#include <QObject>

class ContactListTest : public QObject {
  Q_OBJECT

 private slots:
  void startsEmptyAndRejectsInvalidIndexes();
  void addUserExposesExpectedRoles();
  void removeUserUpdatesRows();
  void updateMethodsChangeDataAndEmitRoles();
  void incrementUnreadCountMovesContactToTop();
  void lookupReturnsContactOrEmptyMap();
};

#endif
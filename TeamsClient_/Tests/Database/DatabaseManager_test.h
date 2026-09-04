#ifndef DATABASE_MANAGER_TEST_H
#define DATABASE_MANAGER_TEST_H

#include <QObject>

class DatabaseManagerTest : public QObject {
  Q_OBJECT

 private slots:
  void instanceIsSingleton();
  void databaseConnectionIsOpen();
  void expectedTablesExist();
  void usersTableHasExpectedSchema();
  void messagesTableHasExpectedSchema();
  void repeatedAccessUsesSameConnection();
  void databaseUsesApplicationDataPath();
  void usersTableHasExpectedConstraints();
  void messagesTableHasExpectedConstraints();
  void databaseSupportsTransactions();
};

#endif
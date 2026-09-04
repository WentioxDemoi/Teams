#ifndef MESSAGE_LIST_TEST_H
#define MESSAGE_LIST_TEST_H

#include <QObject>

class MessageListTest : public QObject {
  Q_OBJECT

 private slots:
  void startsEmpty();
  void addMessageExposesRolesAndUpdatesLastMessage();
  void removeMessageUpdatesRows();
};

#endif
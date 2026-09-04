#ifndef LOCAL_USER_SERVICE_TEST_H
#define LOCAL_USER_SERVICE_TEST_H

#include <QObject>

class LocalUserServiceTest : public QObject {
  Q_OBJECT

 private slots:
  void saveLocalUserUpdatesStateAndEmitsSignal();
  void deleteAllCallsRepository();
};

#endif
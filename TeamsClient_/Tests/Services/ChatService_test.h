#ifndef CHAT_SERVICE_TEST_H
#define CHAT_SERVICE_TEST_H
#include <QObject>
class ChatServiceTest : public QObject {
  Q_OBJECT
 private slots:
  void delegatesMessageOperations();
  void delegatesCallOperations();
  void forwardsSignals();
  void disconnectsBothServices();
  void missingMessageServiceEmitsErrors();
  void missingCallServiceEmitsErrors();
};
#endif
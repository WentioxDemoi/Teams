#ifndef MESSAGE_SERVICE_TEST_H
#define MESSAGE_SERVICE_TEST_H
#include <QObject>
class MessageServiceTest : public QObject {
  Q_OBJECT
 private slots:
  void sendMessagePersistsAndSendsPayload();
  void invalidMessageIsNotSent();
  void incomingMessageIsForwarded();
  void serverErrorIsForwarded();
  void disconnectsNetwork();
  void loadConversationsSendsRequest();
  void deleteAllCallsRepository();
  void conversationsLoadedPersistsAndEmits();
  void malformedServerResponseEmitsError();
};
#endif
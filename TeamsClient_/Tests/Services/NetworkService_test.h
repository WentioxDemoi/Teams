#ifndef NETWORK_SERVICE_TEST_H
#define NETWORK_SERVICE_TEST_H

#include <QObject>

class NetworkServiceTest : public QObject {
  Q_OBJECT

 private slots:
  void emptyPayloadIsIgnored();
  void validJsonIsEmitted();
  void malformedJsonEmitsError();
  void missingTypeEmitsError();
  void newlineDelimitedMessagesAreParsed();
  void emptyIncomingDataIsIgnored();
  void nonObjectJsonEmitsError();
  void invalidTypeEmitsError();
  void nonEmptyPayloadIsQueuedUntilEncrypted();
  void savingUserQueuesAuthenticationPayload();
  void disconnectPreservesPendingQueue();
};

#endif
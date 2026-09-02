#ifndef MESSAGE_TEST_H
#define MESSAGE_TEST_H

#include <QObject>

class MessageTest : public QObject {
  Q_OBJECT

 private slots:
  void defaultMessageIsInvalid();
  void validMessageIsRecognized();
  void jsonRoundTripPreservesFields();
  void emptyUuidIsGenerated();
  void outgoingMessageUsesLocalUser();
};

#endif
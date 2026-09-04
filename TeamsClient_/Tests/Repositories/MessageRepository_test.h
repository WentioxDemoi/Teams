#ifndef MESSAGE_REPOSITORY_TEST_H
#define MESSAGE_REPOSITORY_TEST_H

#include <QObject>

class MessageRepositoryTest : public QObject {
  Q_OBJECT

 private slots:
  void saveAndFindMessageRoundTrip();
  void saveUsesUuidAsUpsertKey();
  void findAllReturnsMessagesInTimestampOrder();
  void findForConversationMatchesBothDirections();
  void findForParticipantMatchesSenderOrReceiver();
  void removeDeletesOnlyRequestedMessage();
  void removeAllClearsMessages();
};

#endif
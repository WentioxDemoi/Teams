#ifndef CHAT_VIEW_MODEL_TEST_H
#define CHAT_VIEW_MODEL_TEST_H

#include <QObject>

class ChatViewModelTest : public QObject {
  Q_OBJECT
 private slots:
  void exposesInjectedModels();
  void selectContactUpdatesSelectionAndReadState();
  void selectUserAddsSearchResultAndSavesContact();
  void searchClearsOnBlankAndDelegatesNonBlank();
  void sendMessageUpdatesModelAndDelegates();
  void invalidMessageIsIgnored();
  void contactsAndConversationsAreLoaded();
  void unknownMessageIsResolvedAndStored();
  void knownMessageUpdatesContactUnreadCount();
  void incomingCallIsShownOrResolved();
  void incomingCallCancellationIsForwarded();
  void statusUpdateRefreshesSelectedContact();
  void applicationQuitDisconnectsServices();
};

#endif
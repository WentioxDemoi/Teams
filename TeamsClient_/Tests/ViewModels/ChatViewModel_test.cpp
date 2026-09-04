#include "ChatViewModel_test.h"

#include "Core/State/SessionState.h"
#include "Models/ContactList.h"
#include "Models/SearchResults.h"
#include "../Services/TestDoubles.h"
#include "ViewModels/ChatViewModel.h"

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

namespace {
User user(const QString& uuid, const QString& status = "Online") {
  return User(uuid + "@example.com", "Alice", uuid, status, false, {}, uuid, {}, {});
}

Message message(const QString& uuid, const QString& sender, const QString& receiver,
                const QString& content, const QDateTime& timestamp = QDateTime::currentDateTimeUtc()) {
  return Message(uuid, sender, receiver, "message", content, timestamp);
}

struct ChatFixture {
  ChatFixture()
      : contacts(), chat(), contactService(), session(), searchResults(), viewModel(&contacts, &chat,
                                                                                      &contactService, &session,
                                                                                      &searchResults) {}
  ContactList contacts;
  FakeChatService chat;
  FakeContactService contactService;
  SessionState session;
  SearchResults searchResults;
  ChatViewModel viewModel;
};
}

void ChatViewModelTest::exposesInjectedModels() {
  ChatFixture fixture;
  QCOMPARE(fixture.viewModel.contactList(), &fixture.contacts);
  QCOMPARE(fixture.viewModel.searchResults(), &fixture.searchResults);
  QVERIFY(fixture.viewModel.messageList() == nullptr);
  QVERIFY(fixture.viewModel.selectedContact().isEmpty());
}

void ChatViewModelTest::selectContactUpdatesSelectionAndReadState() {
  ChatFixture fixture;
  fixture.contacts.addUser(user("user-1"));
  QSignalSpy selectedSpy(&fixture.viewModel, &ChatViewModel::selectedContactChanged);
  QSignalSpy messageListSpy(&fixture.viewModel, &ChatViewModel::messageListChanged);

  fixture.viewModel.selectContact("user-1");

  QCOMPARE(fixture.viewModel.selectedContact().value("uuid").toString(), QStringLiteral("user-1"));
  QVERIFY(fixture.viewModel.messageList() != nullptr);
  QCOMPARE(fixture.contactService.lastReadCalls, 1);
  QCOMPARE(fixture.contactService.lastReadUuid, QStringLiteral("user-1"));
  QCOMPARE(selectedSpy.count(), 1);
  QCOMPARE(messageListSpy.count(), 1);
}

void ChatViewModelTest::selectUserAddsSearchResultAndSavesContact() {
  ChatFixture fixture;
  const User searched = user("user-2");
  fixture.searchResults.addUser(searched);

  fixture.viewModel.selectUser("user-2");

  QCOMPARE(fixture.contacts.rowCount(), 1);
  QCOMPARE(fixture.contactService.saveCalls, 1);
  QCOMPARE(fixture.contactService.savedUser.uuid(), QStringLiteral("user-2"));
  QCOMPARE(fixture.viewModel.selectedContact().value("uuid").toString(), QStringLiteral("user-2"));
  QVERIFY(fixture.viewModel.messageList() != nullptr);
}

void ChatViewModelTest::searchClearsOnBlankAndDelegatesNonBlank() {
  ChatFixture fixture;
  fixture.searchResults.addUser(user("user-1"));

  fixture.viewModel.searchUsers("  ");
  QCOMPARE(fixture.searchResults.rowCount(), 0);
  QCOMPARE(fixture.contactService.searchCalls, 0);
  fixture.viewModel.searchUsers("alice");
  QCOMPARE(fixture.contactService.searchCalls, 1);
  QCOMPARE(fixture.contactService.lastQuery, QStringLiteral("alice"));
}

void ChatViewModelTest::sendMessageUpdatesModelAndDelegates() {
  ChatFixture fixture;
  fixture.contacts.addUser(user("user-1"));
  fixture.viewModel.selectContact("user-1");

  fixture.viewModel.sendMessage(" Hello ");

  QVERIFY(fixture.viewModel.messageList() != nullptr);
  QCOMPARE(fixture.viewModel.messageList()->rowCount(), 1);
  QCOMPARE(fixture.viewModel.messageList()->lastMessage().content(), QStringLiteral(" Hello "));
  QCOMPARE(fixture.chat.sendCalls, 1);
  QCOMPARE(fixture.chat.sentMessage.receiverUuid(), QStringLiteral("user-1"));
  QCOMPARE(fixture.chat.sendCalls, 1);
}

void ChatViewModelTest::invalidMessageIsIgnored() {
  ChatFixture fixture;
  fixture.viewModel.sendMessage("Hello");
  fixture.viewModel.sendMessage("   ");
  QCOMPARE(fixture.chat.sendCalls, 0);
}

void ChatViewModelTest::contactsAndConversationsAreLoaded() {
  ChatFixture fixture;
  const QList<User> contacts{user("user-1"), user("user-2")};
  emit fixture.contactService.contactsLoaded(contacts);

  QCOMPARE(fixture.contacts.rowCount(), 2);
  QCOMPARE(fixture.chat.loadCalls, 1);

  const Message incoming = message("message-1", "user-1", "me", "Hello");
  emit fixture.chat.conversationsLoaded({incoming});
  QCOMPARE(fixture.contacts.get(0).value("lastMessage").toString(), QStringLiteral("Hello"));
  QCOMPARE(fixture.contacts.get(0).value("unreadCount").toInt(), 0);
}

void ChatViewModelTest::unknownMessageIsResolvedAndStored() {
  ChatFixture fixture;
  const Message incoming = message("message-1", "unknown", "me", "Hello");
  emit fixture.chat.messageReceived(incoming);

  QCOMPARE(fixture.contactService.resolveCalls, 1);
  QCOMPARE(fixture.contactService.resolvedUuid, QStringLiteral("unknown"));
  QVERIFY(fixture.viewModel.messageList() == nullptr);

  emit fixture.contactService.userResolved(user("unknown"));
  QCOMPARE(fixture.contacts.rowCount(), 1);
  QCOMPARE(fixture.contacts.get(0).value("lastMessage").toString(), QStringLiteral("Hello"));
  QCOMPARE(fixture.contacts.get(0).value("unreadCount").toInt(), 0);
}

void ChatViewModelTest::knownMessageUpdatesContactUnreadCount() {
  ChatFixture fixture;
  fixture.contacts.addUser(user("user-1"));
  fixture.viewModel.selectContact("user-1");
  fixture.viewModel.selectContact("other");
  emit fixture.chat.messageReceived(message("message-1", "user-1", "me", "Hello"));

  QCOMPARE(fixture.contacts.get(0).value("lastMessage").toString(), QStringLiteral("Hello"));
  QCOMPARE(fixture.contacts.get(0).value("unreadCount").toInt(), 0);
}

void ChatViewModelTest::incomingCallIsShownOrResolved() {
  ChatFixture fixture;
  fixture.contacts.addUser(user("caller-1"));
  QSignalSpy incomingSpy(&fixture.viewModel, &ChatViewModel::incomingCall);
  emit fixture.chat.incomingCallReceived("caller-1");
  QCOMPARE(incomingSpy.count(), 1);
  QCOMPARE(incomingSpy.at(0).at(0).toMap().value("uuid").toString(), QStringLiteral("caller-1"));

  emit fixture.chat.incomingCallReceived("caller-2");
  QCOMPARE(fixture.contactService.resolveCalls, 1);
  QCOMPARE(fixture.contactService.resolvedUuid, QStringLiteral("caller-2"));
  emit fixture.contactService.userResolved(user("caller-2"));
  QCOMPARE(incomingSpy.count(), 2);
}

void ChatViewModelTest::incomingCallCancellationIsForwarded() {
  ChatFixture fixture;
  fixture.contacts.addUser(user("caller-1"));
  QSignalSpy cancelledSpy(&fixture.viewModel, &ChatViewModel::cancelIncomingCall);

  emit fixture.chat.incomingCallCancelled("caller-1");

  QCOMPARE(cancelledSpy.count(), 1);
  QCOMPARE(cancelledSpy.at(0).at(0).toMap().value("uuid").toString(), QStringLiteral("caller-1"));
}

void ChatViewModelTest::statusUpdateRefreshesSelectedContact() {
  ChatFixture fixture;
  fixture.contacts.addUser(user("user-1", "Offline"));
  fixture.viewModel.selectContact("user-1");
  QSignalSpy selectedSpy(&fixture.viewModel, &ChatViewModel::selectedContactChanged);

  emit fixture.contactService.contactStatusUpdated("user-1", "Online");

  QCOMPARE(fixture.viewModel.selectedContact().value("status").toString(), QStringLiteral("Online"));
  QCOMPARE(selectedSpy.count(), 1);
}

void ChatViewModelTest::applicationQuitDisconnectsServices() {
  ChatFixture fixture;

  emit fixture.session.onApplicationQuit();

  QCOMPARE(fixture.chat.disconnectCalls, 1);
  QCOMPARE(fixture.contactService.disconnectCalls, 1);
}
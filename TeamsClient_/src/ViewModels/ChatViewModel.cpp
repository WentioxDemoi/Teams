#include "ChatViewModel.h"

#include <QtCore/Qlist.h>
#include <QtCore/qdebug.h>

#include <QUuid>

#include "../Models/Message.h"
#include "Interfaces/IChatService.h"
#include "Interfaces/IContactService.h"
#include "ModelLocator.h"
#include "ServiceLocator.h"
#include "StateLocator.h"

ChatViewModel::ChatViewModel(ContactList* contactList, IChatService* chatService,
                             IContactService* contactService, SessionState* sessionState,
                             SearchResults* searchResults, QObject* parent)
    : QObject(parent),
      chatService_(chatService ? chatService
                               : ServiceLocator::instance().getService<IChatService>()),
      contactService_(contactService ? contactService
                                     : ServiceLocator::instance().getService<IContactService>()),
      contactList_(contactList ? contactList : ModelLocator::instance().getModel<ContactList>()),
      currentMessageList_(nullptr),
      sessionState_(sessionState ? sessionState
                                 : StateLocator::instance().getState<SessionState>()),
      searchResults_(searchResults ? searchResults : new SearchResults(this)),
      selectedContact_() {
  connect(sessionState_, &SessionState::onApplicationQuit, this, &ChatViewModel::onApplicationQuit);

  connect(chatService_, &IChatService::conversationsLoaded, this, &ChatViewModel::onMessagesLoaded);
  connect(chatService_, &IChatService::connectionUpdate, sessionState_,
          &SessionState::onServerConnectionUpdate);

  connect(contactService_, &IContactService::contactsLoaded, this,
          &ChatViewModel::onContactsLoaded);
  connect(contactService_, &IContactService::connectionUpdate, sessionState_,
          &SessionState::onServerConnectionUpdate);
  connect(contactService_, &IContactService::usersSearchLoaded, searchResults_,
          &SearchResults::onUsersSearchLoaded);
  connect(contactService_, &IContactService::connectionUpdate, sessionState_,
          &SessionState::onServerConnectionUpdate);
}

ContactList* ChatViewModel::contactList() const { return contactList_; }

MessageList* ChatViewModel::messageList() const { return currentMessageList_; }

QVariantMap ChatViewModel::selectedContact() const { return selectedContact_; }

void ChatViewModel::selectContact(const QString& contactUuid) {
  if (!contactList_) return;

  selectedContact_ = contactList_->findByUuid(contactUuid);
  activateConversation(contactUuid);

  emit selectedContactChanged();
  emit messageListChanged();
}

// Dans le cas d'une recherche d'utilisateur sur le serveur via la TopBar
void ChatViewModel::selectUser(const QString& userUuid) {
  if (!contactList_) return;

  selectedContact_ = contactList_->findByUuid(userUuid);

  if (selectedContact_.isEmpty()) {
    const User user = searchResults_->findByUuid(userUuid);
    if (user.isValid()) {
      contactList_->addUser(user);
      contactService_->saveContact(user);
      selectedContact_ = contactList_->toVariantMap(user);
    }
  }

  activateConversation(userUuid);

  emit selectedContactChanged();
  emit messageListChanged();
}

void ChatViewModel::sendMessage(const QString& content) {
  if (!currentMessageList_ || selectedContact_.isEmpty() || content.trimmed().isEmpty()) return;

  const QString recipient = selectedContact_["uuid"].toString();

  Message msg = Message::createOutgoing(recipient, "send_message", content);
  currentMessageList_->addMessage(msg);

  if (chatService_) {
    chatService_->sendMessage(recipient, content);
  }
}

// Ne pas oublier lors du Emit noTokenFound ou en gros lorsque le token est
// invalide de supprimer toute la DB Locale (Contacts + Messages pour le
// moment)
void ChatViewModel::onContactsLoaded(const QList<User>& contacts) {
  for (const User& contact : contacts) {
    contactList_->addUser(contact);
  }
  // chatService_->loadConversationsFromDatabaseAndServer();
}

void ChatViewModel::onMessagesLoaded(const QList<Message>& messages) {
  if (messages.empty()) return;
  messagesByUuid_.clear();

  QString conversationUuid;

  for (const Message& message : messages) {
    if (message.fromMe()) {
      conversationUuid = message.receiverUuid();
    } else {
      conversationUuid = message.senderUuid();
    }

    if (!messagesByUuid_.contains(conversationUuid)) {
      messagesByUuid_[conversationUuid] = new MessageList(this);
    }

    messagesByUuid_[conversationUuid]->addMessage(message);
  }

  selectUser(conversationUuid);
}

void ChatViewModel::searchUsers(const QString& query) {
  qDebug() << "[ChatViewModel] Searching users with query:" << query;
  if (query.trimmed().isEmpty()) {
    searchResults_->clear();
    return;
  }
  contactService_->searchUsers(query);
}

void ChatViewModel::activateConversation(const QString& userUuid) {
  if (!messagesByUuid_.contains(userUuid)) messagesByUuid_.insert(userUuid, new MessageList(this));

  currentMessageList_ = messagesByUuid_[userUuid];
}

void ChatViewModel::onApplicationQuit() {
  qDebug() << "[ChatViewModel] Application is quitting. Performing disconnect.";
  if (chatService_) chatService_->disconnectFromServer();

  if (contactService_) contactService_->disconnectFromServer();
}
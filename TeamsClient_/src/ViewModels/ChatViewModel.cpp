#include "ChatViewModel.h"

#include <QtCore/Qlist.h>
#include <QtCore/qdebug.h>

#include <QUuid>

#include "../Core/State/UserState.h"
#include "../Models/Message.h"
#include "../Repositories/MessageRepository.h" // TMP
#include "Interfaces/IChatService.h"
#include "Interfaces/IContactService.h"
#include "ModelLocator.h"
#include "ServiceLocator.h"

ChatViewModel::ChatViewModel(UserList *userList, IChatService *chatService,
                             IContactService *contactService,
                             SessionState *sessionState,
                             SearchResults *searchResults, QObject *parent)
    : QObject(parent),
      chatService_(chatService
                       ? chatService
                       : ServiceLocator::instance().getService<IChatService>()),
      contactService_(
          contactService
              ? contactService
              : ServiceLocator::instance().getService<IContactService>()),
      userList_(userList ? userList
                         : ModelLocator::instance().getModel<UserList>()),
      currentMessageList_(nullptr),
      sessionState_(sessionState
                        ? sessionState
                        : StateLocator::instance().getState<SessionState>()),
      searchResults_(searchResults ? searchResults : new SearchResults(this)),
      selectedUser_() {
  // TMP
  {
    connect(sessionState_, &SessionState::onApplicationQuit, this,
            &ChatViewModel::onApplicationQuit);
    connect(chatService_, &IChatService::conversationsLoaded, this,
            &ChatViewModel::onMessagesLoaded);
    connect(chatService_, &IChatService::connectionUpdate, sessionState_,
            &SessionState::onServerConnectionUpdate);

    connect(contactService_, &IContactService::contactsLoaded, this,
            &ChatViewModel::onContactsLoaded);
    // connect(contactService_, &IContactService::contactError, this,
    // &IChatService::messageError); // MessageError à créer
    connect(contactService_, &IContactService::connectionUpdate, sessionState_,
            &SessionState::onServerConnectionUpdate);
    connect(contactService_, &IContactService::usersSearchLoaded, this,
            &ChatViewModel::onUsersSearchLoaded);
    connect(contactService_, &IContactService::connectionUpdate, sessionState_,
            &SessionState::onServerConnectionUpdate);
  }
}

UserList *ChatViewModel::userList() const { return userList_; }

MessageList *ChatViewModel::messageList() const { return currentMessageList_; }

QVariantMap ChatViewModel::selectedUser() const { return selectedUser_; }

void ChatViewModel::selectContact(const QString &userUuid) {
  if (!userList_)
    return;

  selectedUser_ = findInUserList(userUuid);
  activateConversation(userUuid);

  emit selectedUserChanged();
  emit messageListChanged();
}

// Dans le cas d'une recherche d'utilisateur sur le serveur via la TopBar
void ChatViewModel::selectUser(const QString &userUuid) {
  if (!userList_)
    return;

  selectedUser_ = findInUserList(userUuid);

  if (selectedUser_.isEmpty()) {
    const User user = searchResults_->findByUuid(userUuid);
    if (user.isValid()) {
      userList_->addUser(user);
      contactService_->saveContact(user);
      selectedUser_ = userList_->toVariantMap(user);
    }
  }

  activateConversation(userUuid);

  emit selectedUserChanged();
  emit messageListChanged();
}

void ChatViewModel::sendMessage(const QString &content) {
  if (!currentMessageList_ || selectedUser_.isEmpty() ||
      content.trimmed().isEmpty())
    return;

  const QString recipient = selectedUser_["uuid"].toString();

  Message msg = Message::createOutgoing(recipient, "send_message", content);
  currentMessageList_->addMessage(msg);

  if (chatService_) {
    chatService_->sendMessage(recipient, content);
  }
}

// Ne pas oublier lors du Emit noTokenFound ou en gros lorsque le token est
// invalide de supprimer toute la DB Locale (Contacts + Messages pour le
// moment)
void ChatViewModel::onContactsLoaded(const QList<User> &users) {
  for (const User &user : users) {
    userList_->addUser(user);
  }
  // chatService_->loadConversationsFromDatabaseAndServer();
}

void ChatViewModel::onMessagesLoaded(const QList<Message> &messages) {
  if (messages.empty())
    return;
  messagesByUuid_.clear();

  QString conversationUuid;

  for (const Message &message : messages) {

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

void ChatViewModel::searchUsers(const QString &query) {
  qDebug() << "[ChatViewModel] Searching users with query:" << query;
  if (query.trimmed().isEmpty()) {
    searchResults_->clear();
    return;
  }
  contactService_->searchUsers(query);
}

void ChatViewModel::onUsersSearchLoaded(const QList<User> users) {
  searchResults_->clear();
  searchResults_->setUsers(users);
}

QVariantMap ChatViewModel::findInUserList(const QString &userUuid) const {
  if (!userList_)
    return {};

  for (int i = 0; i < userList_->rowCount(); ++i) {
    QVariantMap u = userList_->get(i);
    if (u["uuid"].toString() == userUuid) {
      return u;
    }
  }
  return {};
}

void ChatViewModel::activateConversation(const QString &userUuid) {
  if (!messagesByUuid_.contains(userUuid))
    messagesByUuid_.insert(userUuid, new MessageList(this));

  currentMessageList_ = messagesByUuid_[userUuid];
}

void ChatViewModel::onApplicationQuit() {
  qDebug() << "[ChatViewModel] Application is quitting. Performing disconnect.";
  if (chatService_)
    chatService_->disconnectFromServer();

  if (contactService_)
    contactService_->disconnectFromServer();
}
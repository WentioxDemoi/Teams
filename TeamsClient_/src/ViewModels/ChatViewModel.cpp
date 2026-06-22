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
    // Connect saveContact (DB Locale + server)
  }
}

UserList *ChatViewModel::userList() const { return userList_; }

MessageList *ChatViewModel::messageList() const { return currentMessageList_; }

QVariantMap ChatViewModel::selectedUser() const { return selectedUser_; }

// Dans le cas de la selection d'un utilisateur déjà présent dans la liste des
// contacts à gauche
void ChatViewModel::selectContact(const QString &userUuid) {
  if (!userList_)
    return;

  selectedUser_.clear();
  for (int i = 0; i < userList_->rowCount(); ++i) {
    QVariantMap u = userList_->get(i);
    if (u["uuid"].toString() == userUuid) {
      selectedUser_ = u;
      break;
    }
  }

  if (!messagesByUuid_.contains(userUuid))
    messagesByUuid_.insert(userUuid, new MessageList(this));

  currentMessageList_ = messagesByUuid_[userUuid];
  emit selectedUserChanged();
  emit messageListChanged();
}

// Dans le cas d'une recherche d'utilisateur sur le serveur via la TopBar
void ChatViewModel::selectUser(const QString &userUuid) {
  if (!userList_)
    return;

  bool isPresent = false;
  QList<User> searchResultsList;

  selectedUser_.clear();
  for (int i = 0; i < userList_->rowCount(); ++i) {
    QVariantMap u = userList_->get(i);
    if (u["uuid"].toString() == userUuid) {
      selectedUser_ = u;
      isPresent = true;
      break;
    }
  }

  searchResultsList = searchResults_->getSearchResultsList();

  if (!isPresent) {
    for (const User &user : searchResultsList) {
      if (user.uuid() == userUuid) {
        userList_->addUser(user);           // Ajout dans l'UI
        contactService_->saveContact(user); // Ajout dans la DB et le serveur
        for (int i = 0; i < userList_->rowCount(); ++i) {
          QVariantMap u = userList_->get(i);
          if (u["uuid"].toString() == userUuid) {
            selectedUser_ = u;
          }
        }
        break;
      }
    }
  }

  if (!messagesByUuid_.contains(userUuid))
    messagesByUuid_.insert(userUuid, new MessageList(this));

  currentMessageList_ = messagesByUuid_[userUuid];
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


////
//// Ne pas oublier lors du Emit noTokenFound ou en gros lorsque le token est
/// invalide de supprimer toute la DB Locale (Contacts + Messages pour le
/// moment)
void ChatViewModel::onContactsLoaded(const QList<User> &users) {
  for (const User &user : users) {
    userList_->addUser(user);
  }
  // chatService_->loadConversationsFromDatabaseAndServer();
}

// void ChatViewModel::refreshConversationsFromDatabase() {
//   if (!userList_) return;
//   qDebug() << "[ChatViewModel] Refresh conversations from database";
//   const QString localUuid = UserState::instance().localUser().uuid();
//   if (localUuid.isEmpty()) return;

//   qDeleteAll(messagesByUuid_);
//   messagesByUuid_.clear();

//   for (int i = 0; i < userList_->rowCount(); ++i) {
//     QVariantMap contact = userList_->get(i);
//     const QString contactUuid = contact["uuid"].toString();
//     if (contactUuid.isEmpty()) continue;

//     auto* list = new MessageList(this);
//     QList<Message> messages =
//     messageRepository_->findForConversation(localUuid, contactUuid); for
//     (const Message& msg : messages) {
//       list->addMessage(msg);
//     }

//     messagesByUuid_.insert(contactUuid, list);
//   }

//   if (!selectedUser_.isEmpty()) {
//     const QString selectedUuid = selectedUser_["uuid"].toString();
//     if (messagesByUuid_.contains(selectedUuid)) {
//       currentMessageList_ = messagesByUuid_[selectedUuid];
//     }
//   }

//   emit messageListChanged();
// }

void ChatViewModel::onMessagesLoaded(const QList<Message> &messages) {
  qDeleteAll(messagesByUuid_);
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

void ChatViewModel::onApplicationQuit() {
  qDebug() << "[ChatViewModel] Application is quitting. Performing disconnect.";
  chatService_->disconnectFromServer();
  contactService_->disconnectFromServer();
}
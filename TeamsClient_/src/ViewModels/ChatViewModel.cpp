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

ChatViewModel::ChatViewModel(ContactList *contactList, IChatService *chatService, IContactService *contactService,
                             SessionState *sessionState, SearchResults *searchResults, QObject *parent)
    : QObject(parent), chatService_(chatService ? chatService : ServiceLocator::instance().getService<IChatService>()),
      contactService_(contactService ? contactService : ServiceLocator::instance().getService<IContactService>()),
      contactList_(contactList ? contactList : ModelLocator::instance().getModel<ContactList>()),
      currentMessageList_(nullptr),
      sessionState_(sessionState ? sessionState : StateLocator::instance().getState<SessionState>()),
      searchResults_(searchResults ? searchResults : new SearchResults(this)), selectedContact_() {

  connect(sessionState_, &SessionState::onApplicationQuit, this, &ChatViewModel::onApplicationQuit);

  connect(chatService_, &IChatService::conversationsLoaded, this, &ChatViewModel::onConversationsLoaded);
  connect(chatService_, &IChatService::connectionUpdate, sessionState_, &SessionState::onServerConnectionUpdate);
  connect(chatService_, &IChatService::messageReceived, this, &ChatViewModel::onMessageReceived);

  connect(contactService_, &IContactService::contactsLoaded, this, &ChatViewModel::onContactsLoaded);
  connect(contactService_, &IContactService::userResolved, this, &ChatViewModel::onUserResolved);
  connect(contactService_, &IContactService::contactStatusUpdated, this, &ChatViewModel::onContactStatusUpdated);
  connect(contactService_, &IContactService::connectionUpdate, sessionState_, &SessionState::onServerConnectionUpdate);
  connect(contactService_, &IContactService::connectionUpdate, sessionState_, &SessionState::onServerConnectionUpdate);
  connect(contactService_, &IContactService::usersSearchLoaded, searchResults_, &SearchResults::onUsersSearchLoaded);
}

ContactList *ChatViewModel::contactList() const { return contactList_; }

MessageList *ChatViewModel::messageList() const { return currentMessageList_; }

QVariantMap ChatViewModel::selectedContact() const { return selectedContact_; }

// Dnas le cas d'un clique sur un contact dans la liste de contacts sur la gauche
void ChatViewModel::selectContact(const QString &contactUuid) {
  if (!contactList_)
    return;
  if (!selectedContact_.empty())
    contactService_->updateLastReadAt(selectedContact_.value("uuid").toString());
  selectedContact_ = contactList_->findByUuid(contactUuid);
  contactService_->updateLastReadAt(selectedContact_.value("uuid").toString());
  activateConversation(contactUuid);
  contactList_->setUnreadCount(contactUuid, 0);

  emit selectedContactChanged();
  emit messageListChanged();
}

// Dans le cas d'une recherche d'utilisateur sur le serveur via la TopBar
void ChatViewModel::selectUser(const QString &userUuid) {
  if (!contactList_)
    return;

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

void ChatViewModel::sendMessage(const QString &content) {
  if (!currentMessageList_ || selectedContact_.isEmpty() || content.trimmed().isEmpty())
    return;

  Message message = Message::createOutgoing(selectedContact_["uuid"].toString(), "message", content);
  currentMessageList_->addMessage(message);
  contactList_->updateLastMessage(message.receiverUuid(), message.content());
  contactList_->moveToTop(selectedContact_["uuid"].toString());
  if (chatService_) {
    chatService_->sendMessage(message);
  }
}

void ChatViewModel::onContactsLoaded(const QList<User> &contacts) {
  for (const User &contact : contacts) {
    contactList_->addUser(contact);
  }
  // Une fois les conversation loaded, il faudra trouver un moyen d'update le last message de chacun des contacts
  chatService_->loadConversationsFromDatabaseAndServer();
}

// Faudra faire en sorte de ne load que les derniers messages dans le cas d'une authentification par token (qui implique
// des données dans la DB) Si Authentification classique (DB Vierge) faudra faire en sorte de ne load que les derniers
// messages pour éviter une trop grosse charge réseau. Une fois le tirage sur le serveur implémenté, il faudra passer
// dans la fonction la dernière fois que le serveur nous a vu pour pouvoir faire un tri entre les nouveaux messages et
// les messages déjà lu
void ChatViewModel::onConversationsLoaded(const QList<Message> &messages) {
  if (messages.isEmpty())
    return;

  QList<Message> sortedMessages = messages;
  std::stable_sort(sortedMessages.begin(), sortedMessages.end(),
                   [](const Message &a, const Message &b) { return a.timestamp() < b.timestamp(); });

  QHash<QString, int> unreadCounts;

  for (const Message &message : sortedMessages) {
    const QString conversationUuid = message.fromMe() ? message.receiverUuid() : message.senderUuid();

    if (!messagesByUuid_.contains(conversationUuid)) {
      messagesByUuid_.insert(conversationUuid, new MessageList(this));
    }

    messagesByUuid_[conversationUuid]->addMessage(message);
    contactList_->updateLastMessage(conversationUuid, message.content());

    if (!message.fromMe()) {
      const QVariantMap contact = contactList_->findByUuid(conversationUuid);
      const QDateTime lastReadAt = contact.value("lastReadAt").toDateTime().toUTC();

      if (!lastReadAt.isValid() || message.timestamp() > lastReadAt) {
        unreadCounts[conversationUuid] = unreadCounts.value(conversationUuid, 0) + 1;
      }
    }
  }

  for (auto it = unreadCounts.constBegin(); it != unreadCounts.constEnd(); ++it) {
    qDebug() << "[ChatViewModel] Setting unread count for" << it.key() << "=" << it.value();
    contactList_->setUnreadCount(it.key(), it.value());
  }
}

void ChatViewModel::searchUsers(const QString &query) {
  qDebug() << "[ChatViewModel] Searching users with query:" << query;
  if (query.trimmed().isEmpty()) {
    searchResults_->clear();
    return;
  }
  contactService_->searchUsers(query);
}

void ChatViewModel::activateConversation(const QString &userUuid) {
  if (!messagesByUuid_.contains(userUuid))
    messagesByUuid_.insert(userUuid, new MessageList(this));

  currentMessageList_ = messagesByUuid_[userUuid];
}

void ChatViewModel::onMessageReceived(const Message &message) {
  qDebug() << "[ChatViewModel] Message Received";

  const QString senderUuid = message.senderUuid();
  const bool isNewContact = !messagesByUuid_.contains(senderUuid);

  if (isNewContact) {
    messagesByUuid_.insert(senderUuid, new MessageList(this));
    contactService_->resolveUserByUuid(senderUuid);
  }

  messagesByUuid_[senderUuid]->addMessage(message);

  if (isNewContact) {
    pendingLastMessage_[senderUuid] = message;
    pendingUnread_[senderUuid] = pendingUnread_.value(senderUuid, 0) + 1;
  } else {
    contactList_->updateLastMessage(senderUuid, message.content());
    if (senderUuid != selectedContact_["uuid"].toString()) {
      contactList_->incrementUnreadCount(senderUuid);
    }
  }
}

void ChatViewModel::onUserResolved(const User &user) {
  contactList_->addUser(user);

  const QString uuid = user.uuid();

  if (pendingLastMessage_.contains(uuid)) {
    contactList_->updateLastMessage(uuid, pendingLastMessage_[uuid].content());
    pendingLastMessage_.remove(uuid);
  }

  if (pendingUnread_.contains(uuid)) {
    contactList_->setUnreadCount(uuid, pendingUnread_[uuid]);
    pendingUnread_.remove(uuid);
  }

  emit messageListChanged();
}

void ChatViewModel::onContactStatusUpdated(const QString &uuid, const QString &status) {
  if (!contactList_)
    return;

  contactList_->updateStatus(uuid, status);

  if (selectedContact_.value("uuid").toString() == uuid) {
    selectedContact_ = contactList_->findByUuid(uuid);
    emit selectedContactChanged();
    emit messageListChanged();
  }
}

void ChatViewModel::onApplicationQuit() {
  qDebug() << "[ChatViewModel] Application is quitting. Performing disconnect.";
  if (chatService_)
    chatService_->disconnectFromServer();

  if (contactService_)
    contactService_->disconnectFromServer();
}

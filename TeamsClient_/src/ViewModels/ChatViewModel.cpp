#include "ChatViewModel.h"
#include "../Models/Message.h"
#include "Interfaces/IChatService.h"
#include "Interfaces/IContactService.h"
#include "ModelLocator.h"
#include "ServiceLocator.h"
#include "StateLocator.h"

#include <QList>
#include <QDebug>
#include <QUuid>

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
  connect(chatService_, &IChatService::messageReceived, this, &ChatViewModel::onMessageReceived);
  
  connect(chatService_, &IChatService::incomingCallReceived, this, &ChatViewModel::onIncomingCallReceived);
  connect(chatService_, &IChatService::incomingCallCancelled, this, &ChatViewModel::onIncomingCallCancelled);
  

  connect(contactService_, &IContactService::contactsLoaded, this, &ChatViewModel::onContactsLoaded);
  connect(contactService_, &IContactService::userResolved, this, &ChatViewModel::onUserResolved);
  connect(contactService_, &IContactService::contactStatusUpdated, this, &ChatViewModel::onContactStatusUpdated);
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
      // Ici on ne récupère pas le status de l'utilisateur, il faudra faire un call vers le serveur pour récupérer le status de l'utilisateur
      // contactService_-> // Créer un méthode pour récupérer le status de l'utilisateur.
    }
  }

  activateConversation(userUuid);

  emit selectedContactChanged();
  emit messageListChanged();
}

// Envoi d'un message dans la conversation actuellement sélectionnée.
// L'affichage local (currentMessageList_/contactList_) est mis à jour avant
// même la confirmation serveur, chatService_ se charge ensuite de l'envoi réseau.
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

// Callback appelé une fois la liste des contacts reçue (DB locale ou serveur).
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

  // Répartit chaque message dans la conversation correspondante et compte
  // au passage les messages non lus (timestamp plus récent que lastReadAt).
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

// Bascule currentMessageList_ sur la conversation demandée, en la créant
// si elle n'existe pas encore (premier accès à ce contact).
void ChatViewModel::activateConversation(const QString &userUuid) {
  if (!messagesByUuid_.contains(userUuid))
    messagesByUuid_.insert(userUuid, new MessageList(this));

  currentMessageList_ = messagesByUuid_[userUuid];
}

// Réception d'un message en temps réel. Si l'expéditeur n'est pas encore un
// contact connu, le message/compteur non-lu est mis en attente le temps que
// onUserResolved() confirme l'identité du contact.
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

// Callback appelé une fois qu'un uuid inconnu a été résolu en User complet.
// Vide au passage les états en attente (message/unread/appel entrant) posés
// par onMessageReceived() ou onIncomingCallReceived() pour ce même contact.
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

  if (pendingIncomingCallUuid_ == uuid) {
        pendingIncomingCallUuid_.clear();
        emit incomingCall(contactList_->findByUuid(uuid));
    }

  emit messageListChanged();
}

// Mise à jour du statut (online/offline) d'un contact, avec rafraîchissement
// de selectedContact_ si c'est justement le contact affiché à l'écran.
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

// Initie un appel sortant, avec garde-fous côté UI: uuid valide et contact
// effectivement en ligne avant de déléguer à chatService_.
void ChatViewModel::startCall(const QString &contactUuid, const QString &contactUsername) {

if (contactUuid.trimmed().isEmpty()) {
    qDebug() << "UUID du destinataire invalide";
    return;
  }
  if (!(contactList_->findByUuid(contactUuid)["status"] == "Online")) {
    qDebug() << "Contact hors ligne";
    return;
  }
    chatService_->startCall(contactUuid, contactUsername);
  }

// Appel entrant reçu: si le contact est déjà connu on affiche le popup direct,
// sinon on le résout d'abord (voir onUserResolved qui finalise l'affichage).
void ChatViewModel::onIncomingCallReceived(const QString &callerUuid)
{
    if (!contactList_)
        return;

    QVariantMap contact = contactList_->findByUuid(callerUuid);

    if (!contact.isEmpty()) {
      qDebug() << "incoming call\n";
        emit incomingCall(contact);
        return;
    }

    pendingIncomingCallUuid_ = callerUuid;
    contactService_->resolveUserByUuid(callerUuid);
}

void ChatViewModel::onIncomingCallCancelled(const QString &callerUuid) {
  if (!contactList_)
    return;

  // Nettoyage de l'état interne uniquement si l'annulation concerne un appel
  // dont le contact était encore en cours de résolution.
  if (pendingIncomingCallUuid_ == callerUuid) {
    pendingIncomingCallUuid_.clear();
  }

  // Le popup doit se fermer dans tous les cas, que le contact ait été
  // en attente de résolution ou déjà connu et affiché.
  QVariantMap contact = contactList_->findByUuid(callerUuid);
  emit cancelIncomingCall(contact);
}

// Acceptation d'un appel entrant depuis le popup UI.
void ChatViewModel::acceptIncomingCall(const QString &remoteUsername) {
  if (chatService_) {
    chatService_->acceptIncomingCall(remoteUsername);
    pendingIncomingCallUuid_.clear();
  }
}

// Refus d'un appel entrant depuis le popup UI.
void ChatViewModel::rejectIncomingCall() {
  if (chatService_) {
    chatService_->rejectIncomingCall();
    pendingIncomingCallUuid_.clear();
  }
}
#include "ChatViewModel.h"

#include <QtCore/qdebug.h>

#include <QUuid>

#include "../Core/State/UserState.h"
#include "../Models/Message.h"
#include "../Repositories/MessageRepository.h"  // TMP
#include "Interfaces/IChatService.h"
#include "ModelLocator.h"
#include "ServiceLocator.h"


ChatViewModel::ChatViewModel(UserList* userList, IChatService* chatService, SessionState* sessionState, SearchResults* searchResults, QObject* parent)
    : QObject(parent),
      chatService_(chatService ? chatService
                               : ServiceLocator::instance().getService<IChatService>()),
      userList_(userList ? userList : ModelLocator::instance().getModel<UserList>()),
      messageRepository_(new MessageRepository(parent)),
      currentMessageList_(nullptr),
      sessionState_(sessionState ? sessionState : StateLocator::instance().getState<SessionState>()),
      searchResults_(searchResults ? searchResults : new SearchResults(this)),
      selectedUser_() {
  // TMP
  {
    connect(&UserState::instance(), &UserState::localUserSaved, this,
            &ChatViewModel::onLocalUserSaved);
    connect(sessionState_, &SessionState::onApplicationQuit, this, &ChatViewModel::onApplicationQuit);
    connect(chatService_, &IChatService::contactsLoaded, this, &ChatViewModel::onContactsLoaded);
    connect(chatService_, &IChatService::conversationsLoaded, this, &ChatViewModel::onMessagesLoaded);
    connect(chatService_, &IChatService::connectionUpdate, sessionState_, &SessionState::onServerConnectionUpdate);
    
  }
}

UserList* ChatViewModel::userList() const { return userList_; }

MessageList* ChatViewModel::messageList() const { return currentMessageList_; }

QVariantMap ChatViewModel::selectedUser() const { return selectedUser_; }

void ChatViewModel::selectUser(const QString& userUuid) {
  if (!userList_) return;

  selectedUser_.clear();
  for (int i = 0; i < userList_->rowCount(); ++i) {
    QVariantMap u = userList_->get(i);
    if (u["uuid"].toString() == userUuid) {
      selectedUser_ = u;
      break;
    }
  }

  // Créer la MessageList si elle n'existe pas encore
  if (!messagesByUuid_.contains(userUuid)) messagesByUuid_.insert(userUuid, new MessageList(this));

  currentMessageList_ = messagesByUuid_[userUuid];

  emit selectedUserChanged();
  emit messageListChanged();
}

void ChatViewModel::sendMessage(const QString& content) {
  if (!currentMessageList_ || selectedUser_.isEmpty() || content.trimmed().isEmpty()) return;

  const QString recipient = selectedUser_["uuid"].toString();

  // Ajouter le message localement pour l'affichage optimiste
  const QString localUuid = UserState::instance().localUser().uuid();
  QString id = QUuid::createUuid().toString();
  id.remove('{');
  id.remove('}');
  Message msg(id, localUuid.isEmpty() ? "me" : localUuid, recipient, "send_message", content,
              QDateTime::currentDateTime(), true, false);
  currentMessageList_->addMessage(msg);
  persistMessage(msg);

  // Déléguer la création et l'envoi au service
  if (chatService_) {
    chatService_->sendMessage(recipient, content);
  }
}

void ChatViewModel::onLocalUserSaved(const User& user) {
  const QString localUuid = UserState::instance().localUser().uuid();
  if (!localUuid.isEmpty()) {
    seedDatabaseMessages(localUuid); // Pour remplir la DB local de faux messages
  }

  // Recharger les contacts
  if (chatService_) {
    chatService_->loadContactsFromDatabaseAndServer();
  }
}



void ChatViewModel::seedDatabaseMessages(const QString& localUuid) {
  if (localUuid.isEmpty()) return;
qDebug() << "[ChatViewModel] seedDatabaseMessages called with localUuid=" << localUuid;
  auto now = QDateTime::currentDateTime();

  auto insertMessage = [this](const Message& message) {
    if (!messageRepository_->save(message)) {
      qDebug() << "[ChatViewModel] Échec de l'enregistrement du message" << message.uuid();
    }
  };

  insertMessage(Message("m1", "a98ed223-b82a-4f96-b191-dfe5f1a338c0", localUuid, "message",
                        "Salut ! T'as regardé le doc que j'ai partagé ?",
                        now.addSecs(-3600), false, false));
  insertMessage(Message("m2", localUuid, "a98ed223-b82a-4f96-b191-dfe5f1a338c0", "message",
                        "Pas encore, j'ai été pris dans les réunions", now.addSecs(-3400),
                        true, false));
  insertMessage(Message("m3", "a98ed223-b82a-4f96-b191-dfe5f1a338c0", localUuid, "message",
                        "OK je regarde ça ce soir 👍", now.addSecs(-3200), false, false));

  insertMessage(Message("m4", localUuid, "uuid-bob", "message",
                        "Hey, t'as avancé sur le projet ?", now.addSecs(-7200), true, false));
  insertMessage(Message("m5", "uuid-bob", localUuid, "message",
                        "Ouais, presque fini. Il me manque juste un fichier",
                        now.addSecs(-7000), false, false));
  insertMessage(Message("m6", "uuid-bob", localUuid, "message", "Tu peux m'envoyer le fichier ?",
                        now.addSecs(-6800), false, false));

  insertMessage(Message("m7", localUuid, "uuid-clara", "message",
                        "On se retrouve à quelle heure demain ?", now.addSecs(-86400), true,
                        false));
  insertMessage(Message("m8", "uuid-clara", localUuid, "message", "10h ça te va ?",
                        now.addSecs(-86200), false, false));
  insertMessage(Message("m9", localUuid, "uuid-clara", "message", "Nickel !",
                        now.addSecs(-86000), true, false));
  insertMessage(Message("m10", "uuid-clara", localUuid, "message",
                        "Parfait, à demain alors !", now.addSecs(-85800), false, false));

  insertMessage(Message("m11", localUuid, "uuid-brice", "message", "Ça roule ?",
                        now.addSecs(-172800), true, false));
  insertMessage(Message("m12", "uuid-brice", localUuid, "message",
                        "Parfait, à jamais alors !", now.addSecs(-172600), false, false));
}

void ChatViewModel::persistMessage(const Message& message) {
  if (!messageRepository_->save(message)) {
    qDebug() << "[ChatViewModel] Erreur enregistrer le message" << message.uuid();
  }
}


////
//// Ne pas oublier lors du Emit noTokenFound ou en gros lorsque le token est invalide de supprimer toute la DB Locale (Contacts + Messages pour le moment)
void ChatViewModel::onContactsLoaded(const QList<User>& users) {
  for (const User& user : users) {
    userList_->addUser(user);
  }

  chatService_->loadConversationsFromDatabaseAndServer();
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
//     QList<Message> messages = messageRepository_->findForConversation(localUuid, contactUuid);
//     for (const Message& msg : messages) {
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

void ChatViewModel::onMessagesLoaded(const QList<Message>& messages) {
    qDeleteAll(messagesByUuid_);
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
  // appel asynchrone au serveur (NATS/HTTP/whatever),
  // puis dans le callback de réponse :
  QList<User> parsedUsers;
  parsedUsers.append(User("alice@example.com",   "Alice",   "Martin",   "En ligne",  true,  "", "uuid-001", "", "alors"));
  parsedUsers.append(User("bob@example.com",     "Bob",     "Durand",   "Absent",    false, "", "uuid-002", "", NULL));
  parsedUsers.append(User("chloe@example.com",   "Chloé",   "Bernard",  "En ligne",  true,  "", "uuid-003", "", NULL));
  parsedUsers.append(User("david@example.com",   "David",   "Petit",    "Absent",    false, "", "uuid-004", "", NULL));
  parsedUsers.append(User("emma@example.com",    "Emma",    "Robert",   "En ligne",  true,  "", "uuid-005", "", NULL));
  parsedUsers.append(User("felix@example.com",   "Félix",   "Richard",  "Absent",    false, "", "uuid-006", "", NULL));
  parsedUsers.append(User("gabrielle@example.com","Gabrielle","Dubois", "En ligne",  true,  "", "uuid-007", "", NULL));
  parsedUsers.append(User("hugo@example.com",    "Hugo",    "Moreau",   "Absent",    false, "", "uuid-008", "", NULL));
  parsedUsers.append(User("ines@example.com",    "Inès",    "Laurent",  "En ligne",  true,  "", "uuid-009", "", NULL));
  parsedUsers.append(User("jules@example.com",   "Jules",   "Simon",    "Absent",    false, "", "uuid-010", "", NULL));

  searchResults_->setUsers(parsedUsers);
}

void ChatViewModel::onApplicationQuit() {
    qDebug() << "[ChatViewModel] Application is quitting. Performing disconnect.";
    chatService_->disconnectFromServer();
}
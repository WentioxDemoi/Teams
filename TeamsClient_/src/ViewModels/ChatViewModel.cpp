#include "ChatViewModel.h"

#include <QtCore/qdebug.h>

#include <QUuid>

#include "../Core/State/UserState.h"
#include "../Models/Message.h"
#include "../Repositories/MessageRepository.h"  // TMP
#include "Interfaces/IChatService.h"
#include "ModelLocator.h"
#include "ServiceLocator.h"

ChatViewModel::ChatViewModel(UserList* userList, IChatService* chatService, QObject* parent)
    : QObject(parent),
      chatService_(chatService ? chatService
                               : ServiceLocator::instance().getService<IChatService>()),
      userList_(userList ? userList : ModelLocator::instance().getModel<UserList>()),
      messageRepository_(new MessageRepository(parent)),
      currentMessageList_(nullptr),
      selectedUser_() {
  // TMP
  {
    connect(&UserState::instance(), &UserState::localUserSaved, this,
            &ChatViewModel::onLocalUserSaved);
    connect(chatService_, &IChatService::contactsLoaded, this, &ChatViewModel::onContactsLoaded);
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
  Message msg(id, localUuid.isEmpty() ? "me" : localUuid, recipient, "chat", content,
              QDateTime::currentDateTime(), true, false);
  currentMessageList_->addMessage(msg);
  persistMessage(msg);

  // Déléguer la création et l'envoi au service
  if (chatService_) {
    chatService_->sendMessage(recipient, content);
  }
}

void ChatViewModel::onLocalUserSaved(const User& user) {
  // const QString localUuid = UserState::instance().localUser().uuid();
  // if (!localUuid.isEmpty()) {
  //   seedDatabaseMessages(localUuid); // Pour remplir la DB local de faux messages
  // }

  // Recharger les contacts
  if (chatService_) {
    chatService_->loadContacts();
  }
}

void ChatViewModel::refreshConversationsFromDatabase() {
  if (!userList_) return;
  qDebug() << "[ChatViewModel] Refresh conversations from database";
  const QString localUuid = UserState::instance().localUser().uuid();
  if (localUuid.isEmpty()) return;

  qDeleteAll(messagesByUuid_);
  messagesByUuid_.clear();

  for (int i = 0; i < userList_->rowCount(); ++i) {
    QVariantMap contact = userList_->get(i);
    const QString contactUuid = contact["uuid"].toString();
    if (contactUuid.isEmpty()) continue;

    auto* list = new MessageList(this);
    QList<Message> messages = messageRepository_->findForConversation(localUuid, contactUuid);
    for (const Message& msg : messages) {
      list->addMessage(msg);
    }

    messagesByUuid_.insert(contactUuid, list);
  }

  if (!selectedUser_.isEmpty()) {
    const QString selectedUuid = selectedUser_["uuid"].toString();
    if (messagesByUuid_.contains(selectedUuid)) {
      currentMessageList_ = messagesByUuid_[selectedUuid];
    }
  }

  emit messageListChanged();
}

void ChatViewModel::seedDatabaseMessages(const QString& localUuid) {
  if (localUuid.isEmpty()) return;
qDebug() << "[ChatViewModel] seedDatabaseMessages called with localUuid=" << localUuid;
  auto now = QDateTime::currentDateTime();

  auto insertMessage = [this](const Message& message) {
    if (!messageRepository_->insert(message)) {
      qDebug() << "[ChatViewModel] Échec de l'enregistrement du message" << message.uuid();
    }
  };

  insertMessage(Message("m1", "uuid-alice", localUuid, "chat",
                        "Salut ! T'as regardé le doc que j'ai partagé ?",
                        now.addSecs(-3600), false, false));
  insertMessage(Message("m2", localUuid, "uuid-alice", "chat",
                        "Pas encore, j'ai été pris dans les réunions", now.addSecs(-3400),
                        true, false));
  insertMessage(Message("m3", "uuid-alice", localUuid, "chat",
                        "OK je regarde ça ce soir 👍", now.addSecs(-3200), false, false));

  insertMessage(Message("m4", localUuid, "uuid-bob", "chat",
                        "Hey, t'as avancé sur le projet ?", now.addSecs(-7200), true, false));
  insertMessage(Message("m5", "uuid-bob", localUuid, "chat",
                        "Ouais, presque fini. Il me manque juste un fichier",
                        now.addSecs(-7000), false, false));
  insertMessage(Message("m6", "uuid-bob", localUuid, "chat", "Tu peux m'envoyer le fichier ?",
                        now.addSecs(-6800), false, false));

  insertMessage(Message("m7", localUuid, "uuid-clara", "chat",
                        "On se retrouve à quelle heure demain ?", now.addSecs(-86400), true,
                        false));
  insertMessage(Message("m8", "uuid-clara", localUuid, "chat", "10h ça te va ?",
                        now.addSecs(-86200), false, false));
  insertMessage(Message("m9", localUuid, "uuid-clara", "chat", "Nickel !",
                        now.addSecs(-86000), true, false));
  insertMessage(Message("m10", "uuid-clara", localUuid, "chat",
                        "Parfait, à demain alors !", now.addSecs(-85800), false, false));

  insertMessage(Message("m11", localUuid, "uuid-brice", "chat", "Ça roule ?",
                        now.addSecs(-172800), true, false));
  insertMessage(Message("m12", "uuid-brice", localUuid, "chat",
                        "Parfait, à jamais alors !", now.addSecs(-172600), false, false));
}

void ChatViewModel::persistMessage(const Message& message) {
  if (!messageRepository_->insert(message)) {
    qDebug() << "[ChatViewModel] Erreur enregistrer le message" << message.uuid();
  }
}

void ChatViewModel::onContactsLoaded(const QList<User>& users) {
  for (const User& user : users) {
    userList_->addUser(user);
  }

  // Recharger les conversations
  refreshConversationsFromDatabase();
  if (selectedUser_.isEmpty()) {
    selectUser("uuid-alice");
  }
}

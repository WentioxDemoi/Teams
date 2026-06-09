#include "ChatViewModel.h"
#include "ModelLocator.h"

ChatViewModel::ChatViewModel(UserList* userList, QObject* parent)
    : QObject(parent),
      userList_(userList ? userList : ModelLocator::instance().getModel<UserList>()),
      currentMessageList_(nullptr),
      selectedUser_() {

  // ── Fausses conversations ────────────────────────────────────────────
  auto now = QDateTime::currentDateTime();

  auto* alice = new MessageList(this);
  alice->addMessage(Message("m1", "uuid-alice", "Salut ! T'as regardé le doc que j'ai partagé ?", now.addSecs(-3600), false, false));
  alice->addMessage(Message("m2", "me",         "Pas encore, j'ai été pris dans les réunions",    now.addSecs(-3400), true, false));
  alice->addMessage(Message("m3", "uuid-alice", "OK je regarde ça ce soir 👍",                    now.addSecs(-3200), false, false));
  messagesByUuid_.insert("uuid-alice", alice);

  auto* bob = new MessageList(this);
  bob->addMessage(Message("m4", "me",       "Hey, t'as avancé sur le projet ?",      now.addSecs(-7200), true, false));
  bob->addMessage(Message("m5", "uuid-bob", "Ouais, presque fini. Il me manque juste un fichier", now.addSecs(-7000), false, false));
  bob->addMessage(Message("m6", "uuid-bob", "Tu peux m'envoyer le fichier ?",         now.addSecs(-6800), false, false));
  messagesByUuid_.insert("uuid-bob", bob);

  auto* clara = new MessageList(this);
  clara->addMessage(Message("m7",  "me",         "On se retrouve à quelle heure demain ?", now.addSecs(-86400), true, false));
  clara->addMessage(Message("m8",  "uuid-clara", "10h ça te va ?",                          now.addSecs(-86200), false, false));
  clara->addMessage(Message("m9",  "me",         "Nickel !",                                now.addSecs(-86000), true, false));
  clara->addMessage(Message("m10", "uuid-clara", "Parfait, à demain alors !",               now.addSecs(-85800), false, false));
  messagesByUuid_.insert("uuid-clara", clara);

  auto* brice = new MessageList(this);
  brice->addMessage(Message("m11", "me",         "Ça roule ?",                now.addSecs(-172800), true, false));
  brice->addMessage(Message("m12", "uuid-brice", "Parfait, à jamais alors !", now.addSecs(-172600), false, false));
  messagesByUuid_.insert("uuid-brice", brice);

  selectUser("uuid-alice");
}

UserList* ChatViewModel::userList() const {
  return userList_;
}

MessageList* ChatViewModel::messageList() const {
  return currentMessageList_;
}

QVariantMap ChatViewModel::selectedUser() const {
  return selectedUser_;
}

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
  if (!messagesByUuid_.contains(userUuid))
    messagesByUuid_.insert(userUuid, new MessageList(this));

  currentMessageList_ = messagesByUuid_[userUuid];

  emit selectedUserChanged();
  emit messageListChanged();
}

void ChatViewModel::sendMessage(const QString& content) {
  if (!currentMessageList_ || selectedUser_.isEmpty() || content.trimmed().isEmpty()) return;

  // TODO: passer par le service quand il sera implémenté
  Message msg("", "", content, QDateTime::currentDateTime(), true);
  currentMessageList_->addMessage(msg);
}

void ChatViewModel::loadContacts() {
  // TODO: appeler le service pour charger les contacts depuis le serveur
}
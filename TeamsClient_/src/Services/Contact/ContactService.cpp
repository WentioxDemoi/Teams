#include "ContactService.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <cstddef>

#include "../../Models/User.h"

ContactService::ContactService(NetworkService* network, UserRepository* userRepo, QObject* parent)
    : IContactService(parent),
      network_(network ? network : new NetworkService(8084, parent)),
      userRepo_(userRepo ? userRepo : new UserRepository(parent)) {
  Q_ASSERT(network_);
  Q_ASSERT(userRepo_);

  connect(network_, &NetworkService::jsonReceived, this, &ContactService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this, &ContactService::contactError);
  connect(network_, &NetworkService::connectionUpdate, this, &ContactService::connectionUpdate);

  // TMPAlice
}

void ContactService::loadContactsFromDatabaseAndServer() {
  
  // TODO
  // 1. Fetch des contacts depuis le serveur
  // 2. Enregistrement des contacts dans la DB locale (merge)
  // 3. Fetch des contacts depuis la DB locale et émission du signal contactsLoaded


  // A venir pour mettre à jour la liste de contacts depuis le serveur
  //   QJsonObject payload;
  //   payload["type"] = "load_users";
  //   payload["token"] = UserState::instance().localUser().token();
  //   network_->send(payload);

  saveContact(User("alice@example.com", "", "Martin", "En ligne", false, "", "a98ed223-b82a-4f96-b191-dfe5f1a338c0", "",
                   "OK je regarde ça ce soir 👍"));
  saveContact(User("bob@example.com", "Bob", "Dupont", "Absent", false, "", "uuid-bob", "",
                   "Tu peux m'envoyer le fichier ?"));
  saveContact(User("clara@example.com", "Clara", "Roux", "En ligne", false, "", "uuid-clara", "",
                   "Parfait, à demain alors !"));
  saveContact(User("brice@example.com", "Brice", "Roux", "En ligne", false, "", "uuid-brice", "",
                   "Parfait, à jamais  alors !"));
    saveContact(User("test@example.com", "test", "testt", "En ligne", false, "", "uuid-test", "",
                   NULL));

  QList<User> users = userRepo_->findAll();
  
    if (!users.isEmpty()) {
    emit contactsLoaded(users);
  }
}

void ContactService::saveContact(const User& user) {
  if (userRepo_->save(user)) {
    emit contactSaved(user);
  } else {
    emit contactError("Impossible de sauvegarder le contact");
  }
}

void ContactService::deleteContact(const QString& uuid) {
  if (userRepo_->remove(uuid)) {
    emit contactDeleted(uuid);
  } else {
    emit contactError("Impossible de supprimer le contact");
  }
}

void ContactService::deleteAll() {
  if (userRepo_->removeAll()) {
    qDebug() << "Tous les contacts supprimés.";
  } else {
    qDebug() << "Erreur lors de la suppression de tous les contacts.";
  }
}



void ContactService::handleServerResponse(const QJsonObject& root) {
  if (!root.contains("type") || !root["type"].isString()) {
    emit contactError("Réponse serveur contact invalide");
    return;
  }

  const QString type = root["type"].toString();
  if (root.contains("error") && root["error"].isString()) {
    emit contactError(root["error"].toString());
    return;
  }

  if (type == "contacts_response" && root.contains("contacts") && root["contacts"].isArray()) {
    QList<User> users;
    for (const auto& item : root["contacts"].toArray()) {
      if (item.isObject()) {
        users.append(User::fromJson(item.toObject()));
      }
    }
    persistContacts(users);
    
    emit contactsLoaded(users);
    return;
  }
}

// A utiliser uniquement une fois la réponse positive sur l'enregistrement du user reçu.
void ContactService::persistContacts(const QList<User>& users) {
  for (const User& user : users) {
    if (!userRepo_->save(user)) {
      qDebug() << "[ContactService] Impossible de persister le user" << user.uuid();
    }
  }
}

void ContactService::disconnectFromServer() {
    network_->disconnectFromServer();
}
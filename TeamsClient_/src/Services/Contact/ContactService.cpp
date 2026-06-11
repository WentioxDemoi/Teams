#include "ContactService.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

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
}

void ContactService::loadContacts() {
  QList<User> users = userRepo_->findAll();
  if (!users.isEmpty()) {
    emitContactsLoaded(users);
  }

  QJsonObject payload;
  payload["type"] = "load_users";
  network_->send(payload);
}

void ContactService::saveContact(const User& user) {
  if (userRepo_->insert(user)) {
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

void ContactService::disconnectFromServer() {
  if (network_) {
    network_->disconnectFromServer();
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

  if (type == "users_response" && root.contains("users") && root["users"].isArray()) {
    QList<User> users;
    for (const auto& item : root["users"].toArray()) {
      if (item.isObject()) {
        users.append(User::fromJson(item.toObject()));
      }
    }
    persistContacts(users);
    emitContactsLoaded(users);
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
    emitContactsLoaded(users);
    return;
  }
}

void ContactService::persistContacts(const QList<User>& users) {
  for (const User& user : users) {
    if (!userRepo_->insert(user)) {
      qDebug() << "[ContactService] Impossible de persister le user" << user.uuid();
    }
  }
}

void ContactService::emitContactsLoaded(const QList<User>& users) {
  QList<QJsonObject> contactsJson;
  contactsJson.reserve(users.size());
  for (const User& user : users) {
    contactsJson.append(user.toJson());
  }
  emit contactsLoaded(contactsJson);
}

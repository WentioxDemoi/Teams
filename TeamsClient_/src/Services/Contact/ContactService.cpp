#include "ContactService.h"

#include "../../Core/State/UserState.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QtCore/qlogging.h>
#include <cstddef>

#include "../../Models/User.h"
#include "Network/NetworkService.h"
#include "SessionEnum.h"
#include "State/SessionState.h"

ContactService::ContactService(NetworkService *network,
                               UserRepository *userRepo, QObject *parent)
    : IContactService(parent),
      network_(network ? network : new NetworkService(8084, parent)),
      userRepo_(userRepo ? userRepo : new UserRepository(parent)) {
  Q_ASSERT(network_);
  Q_ASSERT(userRepo_);

  connect(network_, &NetworkService::jsonReceived, this,
          &ContactService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this,
          &ContactService::contactError);
  connect(network_, &NetworkService::connectionUpdate, this,
          &ContactService::connectionUpdate);
  connect(&UserState::instance(), &UserState::localUserSaved, this,
          &ContactService::auth);
  

}

void ContactService::loadContactsFromServer() {

  QJsonObject payload;
  payload["type"] = "load_contacts";
  payload["token"] = UserState::instance().localUser().token();
  network_->send(payload);
}

void ContactService::loadContactsFromDatabase() {
  QList<User> users = userRepo_->findAll();

  if (!users.isEmpty()) {
    emit contactsLoaded(users);
  }
}

void ContactService::saveContact(const User &user) {
  if (userRepo_->save(user)) {
    QJsonObject payload;
    payload["type"] = "add_contact";
    payload["token"] = UserState::instance().localUser().token();
    payload["contactUuid"] = user.uuid();

    network_->send(payload);
    // emit contactSaved(user); // Pas utilisé pour le moment
  } else {
    emit contactError("Impossible de sauvegarder le contact");
  }
}

void ContactService::deleteContact(const QString &uuid) {
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

void ContactService::handleServerResponse(const QJsonObject &root) {
  if (!root.contains("type") || !root["type"].isString()) {
    emit contactError("Réponse serveur contact invalide");
    return;
  }
  const QString type = root["type"].toString();
  if (root.contains("error") && root["error"].isString()) {
    emit contactError(root["error"].toString());
    return;
  }

  if (type == "auth_success") {
    qDebug() << "Instantiation de loadContact";
    loadContactsFromServer();
  }

  if (type == "contacts_loaded") {
    if (!root.contains("data") || !root["data"].isArray()) {
      emit contactError("Missing data field in server response");
      return;
    }
    QList<User> users;
    for (const auto &item : root["data"].toArray()) {
      if (item.isObject()) {
        users.append(User::fromJson(item.toObject()));
      }
    }
    persistContacts(users);
    qDebug() << "[ContactService] Contact loaded";
    loadContactsFromDatabase();
    return;
  }

  if (type == "contact_added") {
    qDebug() << "[ContactService] Contact added";
    return;
  }

  if (type == "search_users_response") {
    if (!root.contains("data") || !root["data"].isArray()) {
      emit contactError("Missing data field in server response");
      return;
    }

    QList<User> users;
    for (const auto &item : root["data"].toArray()) {
      if (item.isObject()) {
        users.append(User::fromJson(item.toObject()));
      }
    }
    qDebug() << "[ContactService] Users from search request loaded";
    emit usersSearchLoaded(
        users); // adapte le nom du signal selon ta convention
    return;
  }
}

// A utiliser uniquement une fois la réponse positive sur l'enregistrement du
// user reçu.
void ContactService::persistContacts(const QList<User> &users) {
  for (const User &user : users) {
    if (!userRepo_->save(user)) {
      qDebug() << "[ContactService] Impossible de persister le user"
               << user.uuid();
    }
  }
}

void ContactService::searchUsers(const QString &query) {
  if (query.trimmed().isEmpty()) {
    return;
  }

  QJsonObject payload;
  payload["type"] = "search_users";
  payload["token"] = UserState::instance().localUser().token();
  payload["query"] = query;

  network_->send(payload);
}

void ContactService::auth(const User &user) {
  QJsonObject payload;
  payload["token"] = user.token();
  network_->send(payload);
}

void ContactService::disconnectFromServer() {
  if (network_) {
    network_->disconnectFromServer();
  }
}
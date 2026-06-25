#include "ContactService.h"

#include "../../Core/State/UserState.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QtCore/qdatetime.h>
#include <QtCore/qlogging.h>
#include <QtCore/qnamespace.h>
#include <cstddef>

#include "../../Models/User.h"
#include "Network/NetworkService.h"
#include "SessionEnum.h"
#include "State/SessionState.h"

ContactService::ContactService(NetworkService *network, UserRepository *userRepo, QObject *parent)
    : IContactService(parent), network_(network ? network : new NetworkService(8084, parent)),
      userRepo_(userRepo ? userRepo : new UserRepository(parent)) {
  Q_ASSERT(network_);
  Q_ASSERT(userRepo_);

  connect(network_, &NetworkService::jsonReceived, this, &ContactService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this, &ContactService::contactError);
  connect(network_, &NetworkService::connectionUpdate, this, &ContactService::connectionUpdate);
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
 
void ContactService::updateLastReadAt(const QString &uuid) {
  QJsonObject payload;
  payload["type"] = "update_last_read_at";
  payload["token"] = UserState::instance().localUser().token();
  payload["contactUuid"] = uuid;
  payload["lastReadAt"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
  network_->send(payload);
}

void ContactService::resolveUserByUuid(const QString &uuid) {
  QJsonObject payload;
  payload["type"] = "resolve_user_by_uuid";
  payload["token"] = UserState::instance().localUser().token();
  payload["contactUuid"] = uuid;

  network_->send(payload);
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
    qDebug() << "[ContactService] Enregistrement du contact impossible";
    emit contactError("Impossible de sauvegarder le contact");
  }
}

void ContactService::removeContact(const QString &uuid) {
  if (userRepo_->remove(uuid)) {
    QJsonObject payload;
    payload["type"] = "remove_contact";
    payload["token"] = UserState::instance().localUser().token();
    payload["contactUuid"] = uuid;

    network_->send(payload);
    // emit contactDeleted(uuid); // Pas utilisé pour le moment
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

  const QString type = root["type"].toString();

  if (root.contains("error") && root["error"].isString()) {
    emit contactError(root["error"].toString());
    return;
  }

  if (type == "auth_success" || type == "contact_added" || type == "resolve_user_response") {

    // Uniquement dans ce Handler là car c'est ici qu'on lance l'initialisation du
    // client
    if (type == "auth_success") {
      loadContactsFromServer();
      return;
    }

    // Sécurité pour les payloads contenant un seul éléments (d'où le .toObject).
    if (!root.contains("data") || !root["data"].isObject()) {
      emit contactError("Réponse serveur resolve_user_response invalide");
      return;
    }
    if (type == "contact_added") {
      return;
    }
    if (type == "resolve_user_response") {
      const User user = User::fromJson(root["data"].toObject());

      if (!user.isValid()) {
        emit contactError("Utilisateur résolu invalide");
        return;
      }
      saveContact(user);
      emit userResolved(user);
      return;
    }
  } else if (type == "contacts_loaded" || type == "search_users_response") {

    // Sécurité pour les payloads contenant plusieurs éléments (d'où le .toArray).
    if (!root.contains("data") || !root["data"].isArray()) {
      emit contactError("Missing data field in server response");
      return;
    }

    if (type == "contacts_loaded") {
      QList<User> users = parseUsersArray(root["data"].toArray());
      persistContacts(users);
      loadContactsFromDatabase();
      return;
    }

    if (type == "search_users_response") {
      QList<User> users = parseUsersArray(root["data"].toArray());
      emit usersSearchLoaded(users);
      return;
    }
  }
}

// A utiliser uniquement une fois la réponse positive sur l'enregistrement du
// user reçu.
void ContactService::persistContacts(const QList<User> &users) {
  for (const User &user : users) {
    if (!userRepo_->save(user)) {
      qDebug() << "[ContactService] Impossible de persister le user" << user.uuid();
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

QList<User> ContactService::parseUsersArray(const QJsonArray &array) {
  QList<User> users;
  for (const auto &item : array) {
    if (item.isObject()) {
      users.append(User::fromJson(item.toObject()));
    }
  }
  return users;
}

void ContactService::disconnectFromServer() {
  if (network_) {
    network_->disconnectFromServer();
  }
}
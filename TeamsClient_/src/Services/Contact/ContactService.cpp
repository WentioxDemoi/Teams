#include "ContactService.h"
#include "Core/State/UserState.h"
#include "Models/User.h"
#include "Network/NetworkService.h"
#include "Core/State/SessionState.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDateTime>
#include <QDebug>
#include <QtLogging>
#include <Qt>
#include <cstddef>

ContactService::ContactService(NetworkService *network, UserRepository *userRepo, QObject *parent)
    : IContactService(parent), network_(network ? network : new NetworkService(8084, parent)),
      userRepo_(userRepo ? userRepo : new UserRepository(parent)) {
  Q_ASSERT(network_);
  Q_ASSERT(userRepo_);

  connect(network_, &NetworkService::jsonReceived, this, &ContactService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this, &ContactService::contactError);
  connect(network_, &NetworkService::connectionUpdate, &SessionState::instance(), &SessionState::onServerConnectionUpdate);
}

// Client -> Serveur : demande la liste complète des contacts de l'utilisateur.
// Déclenché automatiquement après "auth_success" (voir handleServerResponse),
// la réponse arrivera en asynchrone via le type "contacts_loaded".
void ContactService::loadContactsFromServer() {

  QJsonObject payload;
  payload["type"] = "load_contacts";
  payload["token"] = UserState::instance().localUser().token();
  network_->send(payload);
}

// Local : recharge les contacts déjà persistés en base, sans solliciter le serveur.
// Utilisé pour rafraîchir l'UI immédiatement après une persistance (voir persistContacts).
void ContactService::loadContactsFromDatabase() {
  QList<User> users = userRepo_->findAll();

  if (!users.isEmpty()) {
    emit contactsLoaded(users);
  }
}

// Local + Client -> Serveur : marque un contact comme "lu" jusqu'à maintenant.
// On écrit d'abord en local (source de vérité pour le compteur de non-lus affiché),
// puis on notifie le serveur seulement si l'écriture locale a réussi, pour éviter
// de désynchroniser client et serveur sur un échec DB.
void ContactService::updateLastReadAt(const QString &uuid) {

  QString timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

  if (userRepo_->updateLastReadAt(uuid, timestamp)) {
    QJsonObject payload;
    payload["type"] = "update_last_read_at";
    payload["token"] = UserState::instance().localUser().token();
    payload["contactUuid"] = uuid;
    payload["lastReadAt"] = timestamp;
    network_->send(payload);
  }
}

// Client -> Serveur : demande au serveur de résoudre un uuid en objet User complet
// (cas où on ne connaît qu'un uuid, ex: contact suggéré ou reçu via un autre flux).
// La réponse ("resolve_user_response") déclenchera saveContact() automatiquement.
void ContactService::resolveUserByUuid(const QString &uuid) {
  QJsonObject payload;
  payload["type"] = "resolve_user_by_uuid";
  payload["token"] = UserState::instance().localUser().token();
  payload["contactUuid"] = uuid;

  network_->send(payload);
}

// Local + Client -> Serveur : ajoute un contact.
// On sauvegarde d'abord en local (optimistic write), et on ne notifie le serveur
// que si la sauvegarde a réussi, pour ne jamais avoir un contact connu du serveur
// mais absent de la base locale.
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

// Local : purge complète des contacts en base (aucun appel serveur).
void ContactService::deleteAll() {
  if (userRepo_->removeAll()) {
    qDebug() << "Tous les contacts supprimés.";
  } else {
    qDebug() << "Erreur lors de la suppression de tous les contacts.";
  }
}

// Serveur -> Client : point d'entrée unique pour toutes les réponses/notifications
// du microservice Contact. Dispatch par "type", même pattern que CallService/MessageService.
void ContactService::handleServerResponse(const QJsonObject &root) {

  const QString type = root["type"].toString();

  if (root.contains("error") && root["error"].isString()) {
    emit contactError(root["error"].toString());
    return;
  }

  if (type == "auth_success" || type == "contact_added" || type == "resolve_user_response" ||
      type == "contact_status_update") {

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
    } else if (type == "resolve_user_response") {
      // Réponse à resolveUserByUuid() : on récupère l'utilisateur résolu,
      // on le persiste comme contact local, puis on notifie l'UI.
      const User user = User::fromJson(root["data"].toObject());

      if (!user.isValid()) {
        emit contactError("Utilisateur résolu invalide");
        return;
      }
      saveContact(user);
      emit userResolved(user);
      return;
    } else if (type == "contact_status_update") {
      // Notification poussée par le serveur (pas une réponse à une requête client) :
      // un contact vient de changer de statut (ex: en ligne/hors ligne), on relaie à l'UI.
      QJsonObject data = root["data"].toObject();

      QString uuid = data["uuid"].toString();
      QString status = data["status"].toString();

      emit contactStatusUpdated(uuid, status);
      return;
    }
  } else if (type == "contacts_loaded" || type == "search_users_response") {

    // Sécurité pour les payloads contenant plusieurs éléments (d'où le .toArray).
    if (!root.contains("data") || !root["data"].isArray()) {
      emit contactError("Missing data field in server response");
      return;
    }

    if (type == "contacts_loaded") {
      // Resynchronisation complète : on parse la réponse serveur, on la persiste en base,
      // puis on recharge depuis la base locale pour garder une seule source de vérité côté UI.
      QList<User> users = parseUsersArray(root["data"].toArray());
      persistContacts(users);
      loadContactsFromDatabase();
      return;
    }

    if (type == "search_users_response") {
      // Réponse à searchUsers() : résultats de recherche, pas persistés (ce ne sont
      // pas encore des contacts), juste relayés à l'UI pour affichage du dropdown.
      QList<User> users = parseUsersArray(root["data"].toArray());
      emit usersSearchLoaded(users);
      return;
    }
  }
}

// Interne : sauvegarde en base une liste d'utilisateurs reçus du serveur.
// A utiliser uniquement une fois la réponse positive sur l'enregistrement du
// user reçu.
void ContactService::persistContacts(const QList<User> &users) {
  for (const User &user : users) {
    user.print();
    if (!userRepo_->save(user)) {
      qDebug() << "[ContactService] Impossible de persister le user" << user.uuid();
    }
  }
}

// Client -> Serveur : recherche de nouveaux utilisateurs (hors liste de contacts existante).
// Ignore les requêtes vides pour éviter de spammer le serveur pendant la saisie.
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

// Interne : convertit le tableau JSON brut reçu du serveur en liste de User typés.
// Les entrées qui ne sont pas des objets JSON valides sont silencieusement ignorées.
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
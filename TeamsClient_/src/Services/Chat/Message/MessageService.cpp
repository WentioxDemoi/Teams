#include "MessageService.h"
#include "Core/State/SessionState.h"
#include "Core/State/UserState.h"
#include "Models/Message.h"
#include "Repositories/MessageRepository.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>
#include <QtLogging>

MessageService::MessageService(NetworkService *network, MessageRepository *messageRepo, QObject *parent)
    : IMessageService(parent), network_(network ? network : new NetworkService(8082, parent)),
      messageRepo_(messageRepo ? messageRepo : new MessageRepository(parent)) {
  Q_ASSERT(network_);
  connect(network_, &NetworkService::jsonReceived, this, &MessageService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this, &MessageService::messageError);
  connect(network_, &NetworkService::connectionUpdate, &SessionState::instance(), &SessionState::onServerConnectionUpdate);

  connect(this, &MessageService::messageReceived, this, &MessageService::saveMessage);
}

// Client -> Serveur : demande explicite de resynchronisation des conversations.
// Le serveur répondra en asynchrone via handleServerResponse (type "conversations_loaded").
void MessageService::loadConversationsFromServer() {
  QJsonObject payload;
  payload["type"] = "load_conversations";
  payload["token"] = UserState::instance().localUser().token();
  network_->send(payload);
}

// Local : recharge les conversations déjà persistées en base, sans solliciter le serveur.
// Utilisé au démarrage de l'appli pour un affichage instantané avant toute réponse réseau.
void MessageService::loadConversationsFromDatabase() {
  QList<Message> messages = messageRepo_->findAll();

  if (!messages.isEmpty()) {
    emit conversationsLoaded(messages);
  }
}



// Client -> Serveur : envoi d'un nouveau message.
// On sauvegarde d'abord en local (optimistic write) avant de le pousser au serveur;
// si la sauvegarde locale échoue, on n'envoie rien pour éviter un message fantôme côté UI.
void MessageService::sendMessage(const Message &message) {
  if (saveMessage(message)) {
    const QJsonObject messageJson = message.toJson();

    QJsonObject payload;
    payload["type"] = "send_message";
    payload["token"] = UserState::instance().localUser().token();
    payload["message"] = messageJson;

    network_->send(payload);
  }
}

// Interne : persiste un message en base via le repository.
// Retourne false si le message est invalide ou si l'écriture échoue, pour permettre
// aux appelants (sendMessage, messageReceived) de ne pas propager un état incohérent.
bool MessageService::saveMessage(const Message &message) {
  if (!message.isValid()) {
    qDebug() << "[MessageService] Message incomplet\n";
    // emit messageError("[MessageService] Message incomplet");
    return false;
  }
  if (messageRepo_->save(message)) {
    return true;
    // emit messageSaved(message); // Pas encore utilisé
  } else {
    qDebug() << "[MessageService] Enregistrement du message impossible";
    // emit messageError("Impossible de sauvegarder le message");
    return false;
  }
}

// Local : purge complète de l'historique de messages en base (aucun appel serveur).
void MessageService::deleteAll() {
  if (messageRepo_->removeAll()) {
    qDebug() << "[MessageService] Tous les messages supprimés.";
  } else {
    qDebug() << "[MessageService] Erreur lors de la suppression de tous les messages.";
  }
}

void MessageService::handleServerResponse(const QJsonObject &root) {
  const QString type = root["type"].toString();

  if (root.contains("error") && root["error"].isString()) {
    emit messageError(root["error"].toString());
    return;
  }

  if (type == "message_sent" || type == "new_message") {
    // Sécurité pour les payloads contenant un seul éléments (d'où le .toObject).
    if (!root.contains("data") || !root["data"].isObject()) {
      emit messageError("[MessageService] Réponse serveur new_message invalide");
      return;
    }

    if (type == "message_sent") {
      const Message message = Message::fromJson(root["data"].toObject());

      // emit messageSent(message); // Plus tard, grâce à ce signal, on pourra vérifier que le message ayant été affiché
      // et ayant un uuid temporaire a bien été envoyé et ainsi remplacer son uuid temporaire
      return;
    }

    if (type == "new_message") {
      // Un autre utilisateur nous envoie un message : on le fait remonter à l'UI
      // et il sera persisté automatiquement via la connexion messageReceived -> saveMessage.
      emit messageReceived(Message::fromJson(root["data"].toObject()));
      return;
    }
  } else if (type == "conversations_loaded") {
    // Sécurité pour les payloads contenant plusieurs éléments (d'où le .toArray).
    if (!root.contains("data") || !root["data"].isArray()) {
      emit messageError("[MessageService] Missing data field in server response");
      return;
    }
    if (type == "conversations_loaded") {
      // Resynchronisation complète : on parse la réponse serveur, on la persiste en base,
      // puis on recharge depuis la base locale pour garder une seule source de vérité côté UI.
      QList<Message> conversations = parseMessagesArray(root["data"].toArray());
      persistMessages(conversations);
      loadConversationsFromDatabase();
      return;
    }
  }
}

// Interne : sauvegarde en base une liste de messages reçus du serveur.
// Les échecs individuels sont loggués mais n'interrompent pas le reste du batch.
void MessageService::persistMessages(const QList<Message> &messages) {
  for (const Message &message : messages) {
    if (!messageRepo_->save(message)) {
      qDebug() << "[MessageService] Impossible de persister le message" << message.uuid();
    }
  }
}

// Interne : convertit le tableau JSON brut reçu du serveur en liste de Message typés.
// Les entrées qui ne sont pas des objets JSON valides sont silencieusement ignorées.
QList<Message> MessageService::parseMessagesArray(const QJsonArray &array) {
  QList<Message> conversations;
  for (const auto &item : array) {
    if (item.isObject()) {
      conversations.append(Message::fromJson(item.toObject()));
    }
  }
  return conversations;
}

void MessageService::disconnectFromServer() { network_->disconnectFromServer(); }
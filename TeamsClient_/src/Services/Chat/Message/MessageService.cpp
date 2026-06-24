#include "MessageService.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>
#include <QtCore/qlogging.h>
#include <iostream>

#include "../../Core/State/UserState.h"
#include "../../Models/Message.h"
#include "Repositories/MessageRepository.h"
#include "ServiceLocator.h"

MessageService::MessageService(NetworkService *network, MessageRepository *messageRepo, QObject *parent)
    : IMessageService(parent), network_(network ? network : new NetworkService(8082, parent)),
      messageRepo_(messageRepo ? messageRepo : new MessageRepository(parent)) {
  Q_ASSERT(network_);
  connect(network_, &NetworkService::jsonReceived, this, &MessageService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this, &MessageService::messageError);
  connect(network_, &NetworkService::connectionUpdate, this, &MessageService::connectionUpdate);

  connect(this, &MessageService::messageReceived, this, &MessageService::saveMessage);
}

void MessageService::loadConversationsFromDatabase() {
  // TODO
  // 1. Fetch des messages depuis le serveur
  // 2. Enregistrement des messages dans la DB locale (merge)
  // 3. Fetch des messages depuis la DB locale et émission du signal
  // conversationsLoaded

  // A venir pour mettre à jour la liste de messages depuis le serveur
  //   QJsonObject payload;
  //   payload["type"] = "load_messages";
  //   payload["token"] = UserState::instance().localUser().token();
  //   network_->send(payload);

  QList<Message> messages = messageRepo_->findAll();

  if (!messages.isEmpty()) {
    emit conversationsLoaded(messages);
  }
}

void MessageService::loadConversationsFromServer() {
    QJsonObject payload;
    payload["type"] = "load_conversations";
    payload["token"] = UserState::instance().localUser().token();
    network_->send(payload);
}

void MessageService::sendMessage(const Message &message) {
  if (saveMessage(message)) {
    const QJsonObject messageJson = message.toJson();

    QJsonObject payload;
    payload["type"] = "send_message";
    payload["token"] = UserState::instance().localUser().token();
    payload["message"] = messageJson;

    QJsonDocument doc(payload);
    qDebug() << "=== Envoi de message ===";
    qDebug().noquote() << doc.toJson(QJsonDocument::Indented);
    qDebug() << "=== Fin envoi ===";

    network_->send(payload);
  }
}

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

void MessageService::deleteMessage(const QString &uuid) {
  // if (messageRepo_->remove(uuid)) {
  //   emit messageDeleted(uuid); // Pas encore utilisé
  // } else {
  //   emit messageError("Impossible de supprimer le message");
  // }
}

void MessageService::deleteAll() {
  if (messageRepo_->removeAll()) {
    qDebug() << "Tous les messages supprimés.";
  } else {
    qDebug() << "Erreur lors de la suppression de tous les messages.";
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
      QList<Message> conversations = parseMessagesArray(root["data"].toArray());
      persistMessages(conversations);
      loadConversationsFromDatabase();
      return;
    }
  }
}

void MessageService::persistMessages(const QList<Message> &messages) {
  for (const Message &message : messages) {
    if (!messageRepo_->save(message)) {
      qDebug() << "[MessageService] Impossible de persister le message" << message.uuid();
    }
  }
}

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
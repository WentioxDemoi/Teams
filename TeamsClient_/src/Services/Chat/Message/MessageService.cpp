#include "MessageService.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>
#include <iostream>

#include "../../Core/State/UserState.h"
#include "../../Models/Message.h"
#include "Repositories/MessageRepository.h"
#include "ServiceLocator.h"

MessageService::MessageService(NetworkService *network,
                               MessageRepository *messageRepo, QObject *parent)
    : IMessageService(parent),
      network_(network ? network : new NetworkService(8082, parent)),
      messageRepo_(messageRepo ? messageRepo : new MessageRepository(parent)) {
  Q_ASSERT(network_);
  connect(network_, &NetworkService::jsonReceived, this,
          &MessageService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this,
          &MessageService::messageError);
  connect(network_, &NetworkService::connectionUpdate, this,
          &MessageService::connectionUpdate);
}

void MessageService::loadConversationsFromDatabaseAndServer() {
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

void MessageService::sendMessage(const QString &recipientUuid,
                                 const QString &content) {
  if (recipientUuid.trimmed().isEmpty() || content.trimmed().isEmpty()) {
    emit messageError("Destinataire ou contenu invalide");
    return;
  }

  const QString messageUuid = "";
  const QString senderUuid = "";
  const QDateTime timestamp = QDateTime::currentDateTime();

  Message msg(messageUuid, senderUuid, recipientUuid, "message", content,
              timestamp, true, false);
  const QJsonObject messageJson = msg.toJson();

  if (!messageJson.contains("uuid") || !messageJson.contains("senderUuid") ||
      !messageJson.contains("receiverUuid")) {
    emit messageError("Message construit incomplet");
    return;
  }

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

void MessageService::saveMessage(const Message &message) {
  if (messageRepo_->save(message)) {
    emit messageSaved(message);
  } else {
    emit messageError("Impossible de sauvegarder le message");
  }
}

void MessageService::deleteMessage(const QString &uuid) {
  if (messageRepo_->remove(uuid)) {
    emit messageDeleted(uuid);
  } else {
    emit messageError("Impossible de supprimer le message");
  }
}

void MessageService::deleteAll() {
  if (messageRepo_->removeAll()) {
    qDebug() << "Tous les messages supprimés.";
  } else {
    qDebug() << "Erreur lors de la suppression de tous les messages.";
  }
}

// A REFACTOR POUR ÊTRE SUR LA MËME BASE QUE DANS AUTH OU CONTACT SERVICE (Pour le template des messages reçus en tout cas)

void MessageService::handleServerResponse(const QJsonObject& root) {
  const QString type = root["type"].toString();

  if (root.contains("error") && root["error"].isString()) {
    emit messageError(root["error"].toString());
    return;
  }

  if (type == "message_sent") {
    if (root.contains("status") && root["status"].isString() &&
        root["status"].toString() != "success") {
      emit messageError("Échec de l'envoi du message");
      return;
    }

    if (!root.contains("message") || !root["message"].isObject()) {
      emit messageError("Réponse serveur message_sent invalide");
      return;
    }

    const QJsonObject messageJson = root["message"].toObject();
    const Message message = Message::fromJson(messageJson);

    if (message.uuid().isEmpty() || message.senderUuid().isEmpty() ||
        message.receiverUuid().isEmpty()) {
      emit messageError("Message serveur invalide");
      return;
    }

    persistMessages({message});
    emit messageSent(messageJson);
    return;
  }

  if (type == "new_message") {
    if (!root.contains("data") || !root["data"].isObject()) {
      emit messageError("Réponse serveur new_message invalide");
      return;
    }

    emit messageReceived(root["data"].toObject());
    return;
  }
}

void MessageService::persistMessages(const QList<Message> &messages) {
  for (const Message &message : messages) {
    if (!messageRepo_->save(message)) {
      qDebug() << "[MessageService] Impossible de persister le message"
               << message.uuid();
    }
  }
}

void MessageService::disconnectFromServer() {
  network_->disconnectFromServer();
}
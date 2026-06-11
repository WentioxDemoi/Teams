#include "MessageService.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>

#include "../../Core/State/UserState.h"
#include "../../Models/Message.h"
#include "Repositories/MessageRepository.h"
#include "ServiceLocator.h"

MessageService::MessageService(NetworkService* network, MessageRepository* messageRepo,
                               QObject* parent)
    : IMessageService(parent),
      network_(network ? network : new NetworkService(8082, parent)),
      messageRepo_(messageRepo ? messageRepo : new MessageRepository(parent)) {
  Q_ASSERT(network_);
  connect(network_, &NetworkService::jsonReceived, this, &MessageService::handleServerResponse);
  connect(network_, &NetworkService::networkError, this, &MessageService::messageError);
  connect(network_, &NetworkService::connectionUpdate, this, &MessageService::connectionUpdate);
}

void MessageService::sendMessage(const QString& recipientUuid, const QString& content) {
  if (recipientUuid.trimmed().isEmpty() || content.trimmed().isEmpty()) {
    emit messageError("Destinataire ou contenu invalide");
    return;
  }

  // Construire l'objet Message
  const QString messageUuid = QUuid::createUuid().toString().remove('{').remove('}');
  const QString senderUuid = UserState::instance().localUser().uuid();
  const QString type = "chat";  // par défaut
  const QDateTime timestamp = QDateTime::currentDateTime();

  Message msg(messageUuid, senderUuid, recipientUuid, type, content, timestamp, true, false);
  const QJsonObject messageJson = msg.toJson();

  // Valider le message construit
  if (!messageJson.contains("uuid") || !messageJson.contains("senderUuid") ||
      !messageJson.contains("receiverUuid")) {
    emit messageError("Message construit incomplet");
    return;
  }

  const QString token = UserState::instance().localUser().token();

  // Construire le payload et envoyer au network
  QJsonObject payload;
  payload["type"] = "send_message";
  payload["token"] = token;
  payload["message"] = messageJson;

  // Logger avant d'envoyer
  QJsonDocument doc(payload);
  qDebug() << "=== Envoi de message ===";
  qDebug().noquote() << doc.toJson(QJsonDocument::Indented);
  qDebug() << "=== Fin envoi ===";

  network_->send(payload);
}

void MessageService::loadConversationsFromDatabaseAndServer() {
  // Implementation for loading conversations from database and server
  QList<Message> messages = messageRepo_->findAll();  // Charger d'abord depuis la base locale

    // A venir pour mettre à jour la liste de messages depuis le serveur
  //   QJsonObject payload;
  //   payload["type"] = "load_messages";
  //   payload["token"] = UserState::instance().localUser().token();
  //   network_->send(payload);

  emit conversationsLoaded(messages);
}

void MessageService::disconnectFromServer() { network_->disconnectFromServer(); }

void MessageService::handleServerResponse(const QJsonObject& root) {
  if (!root.contains("type") || !root["type"].isString()) {
    emit messageError("Réponse serveur manquante ou invalide");
    return;
  }

  const QString type = root["type"].toString();
  if (root.contains("error") && root["error"].isString()) {
    emit messageError(root["error"].toString());
    return;
  }

  if (type == "message_sent") {
    if (root.contains("status") && root["status"].isString() &&
        root["status"].toString() != "success") {
      const QString error = root.contains("error") && root["error"].isString()
                                ? root["error"].toString()
                                : "Échec de l'envoi du message";
      emit messageError(error);
      return;
    }

    if (!root.contains("message") || !root["message"].isObject()) {
      emit messageError("Réponse serveur message_sent invalide");
      return;
    }

    QJsonObject messageJson = root["message"].toObject();
    Message message = Message::fromJson(messageJson);

    if (message.uuid().isEmpty() || message.senderUuid().isEmpty() ||
        message.receiverUuid().isEmpty()) {
      emit messageError("Message serveur invalide");
      return;
    }

    if (messageRepo_) {
      messageRepo_->save(message);
    }

    emit messageSent(messageJson);
    return;
  }

  if (type == "message_received" && root.contains("message") && root["message"].isObject()) {
    emit messageReceived(root["message"].toObject());
    return;
  }

  // TO Determine parce que cette partie doit être un fetch de toutes les convs demandés par le client local (en fonction des contacts demandés au serveur)
  if (type == "conversation_response" && root.contains("userUuid") && root.contains("messages") &&
      root["messages"].isArray()) {
    QJsonArray messagesArray = root["messages"].toArray();
    QList<Message> messages;

    for (const QJsonValue& value : messagesArray) {
      if (!value.isObject()) {
        continue;
      }
      Message message = Message::fromJson(value.toObject());
      messages.append(message);
    }

    emit conversationsLoaded(messages);
    return;
  }
}

void MessageService::deleteAll() {
   if (messageRepo_->removeAll()) {
    qDebug() << "Tous les messages supprimés.";
  } else {
    qDebug() << "Erreur lors de la suppression de tous les messages.";
  }
}
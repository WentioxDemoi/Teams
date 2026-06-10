#include "MessageService.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>

#include "../../Core/State/UserState.h"
#include "../../Models/Message.h"

MessageService::MessageService(NetworkService* network, QObject* parent)
    : IMessageService(parent),
      network_(network ? network : new NetworkService(8082, parent)) {
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
  if (!messageJson.contains("uuid") || !messageJson.contains("senderUuid") || !messageJson.contains("receiverUuid")) {
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

void MessageService::loadConversation(const QString& userUuid) {
  if (userUuid.trimmed().isEmpty()) {
    emit messageError("UUID de conversation invalide");
    return;
  }

  network_->send({{"type", "load_conversation"}, {"userUuid", userUuid}});
}

void MessageService::loadContacts() {
  network_->send({{"type", "load_contacts"}});
}

void MessageService::disconnectFromServer() {
  network_->disconnectFromServer();
}

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

  // TO BE DETERMINED
//   if (type == "message_sent" && root.contains("message") && root["message"].isObject()) {
//     emit messageSent(root["message"].toObject());
//     return;
//   }

  if (type == "message_received" && root.contains("message") && root["message"].isObject()) {
    emit messageReceived(root["message"].toObject());
    return;
  }

  if (type == "conversation_response" && root.contains("userUuid") && root.contains("messages") && root["messages"].isArray()) {
    QList<QJsonObject> messages;
    for (const auto& item : root["messages"].toArray()) {
      if (item.isObject()) {
        messages.append(item.toObject());
      }
    }
    emit conversationLoaded(root["userUuid"].toString(), messages);
    return;
  }

  // A voir si depuis le serveur on pack aussi les messages retrouvés
  if (type == "contacts_response" && root.contains("contacts") && root["contacts"].isArray()) {
    QList<QJsonObject> contacts;
    for (const auto& item : root["contacts"].toArray()) {
      if (item.isObject()) {
        contacts.append(item.toObject());
      }
    }
    emit contactsLoaded(contacts);
    return;
  }
}

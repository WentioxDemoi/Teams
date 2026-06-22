#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include <QJsonObject>
#include <QObject>
#include "../Interfaces/IMessageService.h"
#include "../Network/NetworkService.h"
#include "../../Repositories/MessageRepository.h"
#include "../../Core/State/UserState.h"

/**
 * @class MessageService
 * @brief Service de communication pour la messagerie.
 *
 * Envoie et reçoit les données de chat via NetworkService sur le port 8082.
 */
class MessageService : public IMessageService {
  Q_OBJECT
 public:
  explicit MessageService(NetworkService* network = nullptr,
                          MessageRepository* messageRepo = nullptr,
                          QObject* parent = nullptr);

  void loadConversationsFromDatabaseAndServer() override;
  void sendMessage(const QString& recipientUuid, const QString& content) override;
  void saveMessage(const Message& message) override;
  void deleteMessage(const QString& uuid) override;
  void deleteAll() override;
  void disconnectFromServer() override;
  void handleServerResponse(const QJsonObject& root);

 private:
  void persistMessages(const QList<Message>& messages);

  NetworkService* network_;
  MessageRepository* messageRepo_;
};

#endif
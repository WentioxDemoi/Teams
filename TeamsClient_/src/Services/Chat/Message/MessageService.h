#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "Repositories/MessageRepository.h"
#include "Interfaces/IMessageService.h"
#include "Network/NetworkService.h"

#include <QJsonObject>
#include <QObject>

/**
 * @class MessageService
 * @brief Service de communication pour la messagerie.
 *
 * Envoie et reçoit les données de chat via NetworkService sur le port 8082.
 */
class MessageService : public IMessageService {
  Q_OBJECT
public:
  explicit MessageService(NetworkService *network = nullptr, MessageRepository *messageRepo = nullptr,
                          QObject *parent = nullptr);

  void loadConversationsFromServer() override;
  void sendMessage(const Message &message) override;

  void disconnectFromServer() override;
  void deleteAll() override;

private:
  bool saveMessage(const Message &message);

  void handleServerResponse(const QJsonObject &root);
  void loadConversationsFromDatabase();
  void persistMessages(const QList<Message> &messages);
  QList<Message> parseMessagesArray(const QJsonArray &array);

  NetworkService *network_;
  MessageRepository *messageRepo_;
};

#endif
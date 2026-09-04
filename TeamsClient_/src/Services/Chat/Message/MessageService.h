#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "Repositories/IMessageRepository.h"
#include "Interfaces/IMessageService.h"
#include "Network/INetworkService.h"

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
  explicit MessageService(INetworkService *network = nullptr, IMessageRepository *messageRepo = nullptr,
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

  INetworkService *network_;
  IMessageRepository *messageRepo_;
};

#endif
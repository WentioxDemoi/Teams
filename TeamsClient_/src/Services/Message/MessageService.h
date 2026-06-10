#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include <QJsonObject>
#include <QObject>
#include "../Interfaces/IMessageService.h"
#include "../Network/NetworkService.h"

/**
 * @class MessageService
 * @brief Service de communication pour la messagerie.
 *
 * Envoie et reçoit les données de chat via NetworkService sur le port 8082.
 */
class MessageService : public IMessageService {
  Q_OBJECT
 public:
  explicit MessageService(NetworkService* network = nullptr, QObject* parent = nullptr);

 public slots:
  void sendMessage(const QString& recipientUuid, const QString& content) override;
  void loadConversation(const QString& userUuid) override;
  void loadContacts() override;
  void disconnectFromServer() override;

 private slots:
  void handleServerResponse(const QJsonObject& root);

 private:
  NetworkService* network_;
};

#endif

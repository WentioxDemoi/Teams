#ifndef IMESSAGESERVICE_H
#define IMESSAGESERVICE_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>
#include "Message.h"
#include "SessionEnum.h"

/**
 * @class IMessageService
 * @brief Interface pour la messagerie.
 *
 * Définit l'API que doit exposer un service de messagerie pour envoyer,
 * charger et recevoir des messages via le backend.
 */
class IMessageService : public QObject {
  Q_OBJECT
 public:
  explicit IMessageService(QObject* parent = nullptr) : QObject(parent) {};
  virtual ~IMessageService() = default;

 public slots:
  virtual void sendMessage(const QString& recipientUuid, const QString& content) = 0;
  virtual void loadConversationsFromDatabaseAndServer() = 0;
  virtual void disconnectFromServer() = 0;
  virtual void deleteAll() = 0;

 signals:
  void messageSent(const QJsonObject& message);
  void messageReceived(const QJsonObject& message);
  void conversationsLoaded(const QList<Message>& messages);
  void contactsLoaded(const QList<QJsonObject>& contacts);
  void messageError(const QString& error);
  void connectionUpdate(ServerType server, bool status);
};

#endif

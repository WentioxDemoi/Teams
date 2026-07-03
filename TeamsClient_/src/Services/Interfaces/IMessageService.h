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

  virtual void loadConversationsFromServer() = 0;
  virtual void sendMessage(const Message& message) = 0;

  virtual void disconnectFromServer() = 0;
  virtual void deleteAll() = 0;

 signals:
  void conversationsLoaded(const QList<Message>& messages);
  void messageError(const QString& error);
  void messageReceived(const Message& message);
};

#endif
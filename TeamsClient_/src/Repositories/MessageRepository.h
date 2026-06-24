#ifndef MESSAGEREPOSITORY_H
#define MESSAGEREPOSITORY_H

#include <QList>
#include <QSqlDatabase>
#include <QObject>
#include <QString>
#include <optional>

#include "../Models/Message.h"

/**
 * @class MessageRepository
 * @brief Répertoire d'accès aux données de messages.
 *
 * Stocke les messages localement avec toutes les métadonnées nécessaires
 * (senderUuid, receiverUuid, type, content, timestamp, isRead).
 */
class MessageRepository : public QObject {
  Q_OBJECT
 public:
  explicit MessageRepository(QObject* parent = nullptr);

  std::optional<Message> findByUUID(const QString& uuid);
  QList<Message> findAll();
  QList<Message> findForConversation(const QString& userUuid1, const QString& userUuid2);
  QList<Message> findForParticipant(const QString& participantUuid);

  bool save(const Message& message);
  bool remove(const QString& uuid);
  bool removeAll();

 signals:
  void error(const QString& message);

 private:
  QSqlDatabase db_;
};

#endif

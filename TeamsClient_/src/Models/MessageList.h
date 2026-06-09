#ifndef MESSAGELIST_H
#define MESSAGELIST_H

#include <QAbstractListModel>

#include "Message.h"

class MessageList : public QAbstractListModel {
  Q_OBJECT

 public:
  enum Roles {
    UuidRole = Qt::UserRole + 1,
    SenderUuidRole,
    ContentRole,
    TimestampRole,
    IsReadRole,
    FromMeRole,
  };

  explicit MessageList(QObject* parent = nullptr) : QAbstractListModel(parent) {}

  void addMessage(const Message& message);
  void removeMessage(const Message& message);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  Message lastMessage() const;

 private:
  QList<Message> messages_;
};

#endif
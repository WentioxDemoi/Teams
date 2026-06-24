#include "MessageList.h"

int MessageList::rowCount(const QModelIndex& parent) const {
  if (parent.isValid()) return 0;
  return messages_.size();
}

QHash<int, QByteArray> MessageList::roleNames() const {
  return {
      {UuidRole, "uuid"},           {SenderUuidRole, "senderUuid"}, {ContentRole, "content"},
      {TimestampRole, "timestamp"}, {FromMeRole,     "fromMe"},
  };
}

QVariant MessageList::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() >= messages_.size()) return QVariant();

  const Message& message = messages_.at(index.row());

  switch (role) {
    case UuidRole:
      return message.uuid();
    case SenderUuidRole:
      return message.senderUuid();
    case ContentRole:
      return message.content();
    case TimestampRole:
      return message.timestamp();
    case FromMeRole:
      return message.fromMe();
    default:
      qDebug() << "MessageList: Undefined role.";
  }
  return QVariant();
}

void MessageList::addMessage(const Message& message) {
  beginInsertRows(QModelIndex(), messages_.size(), messages_.size());
  messages_.append(message);
  endInsertRows();
}

void MessageList::removeMessage(const Message& message) {
  int row = messages_.indexOf(message);
  if (row < 0) return;
  beginRemoveRows(QModelIndex(), row, row);
  messages_.removeAt(row);
  endRemoveRows();
}

Message MessageList::lastMessage() const {
  if (messages_.isEmpty()) return Message();
  return messages_.last();
}
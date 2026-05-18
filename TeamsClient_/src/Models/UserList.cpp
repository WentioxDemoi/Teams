#include "UserList.h"

#include <QtCore/qdebug.h>
#include <QtCore/qvariant.h>

void UserList::addUser(const User& user) {
  beginInsertRows(QModelIndex(), users_.size(), users_.size());
  users_.append(user);
  endInsertRows();
}

void UserList::removeUser(const User& user) {
  int row = users_.indexOf(user);
  if (row < 0) return;
  beginRemoveRows(QModelIndex(), row, row);
  users_.removeAt(row);
  endRemoveRows();
}

QHash<int, QByteArray> UserList::roleNames() const {
  return {
      {UsernameRole, "username"},
      {StatusRole, "status"},
      {AvatarRole, "avatar"},
      {LastMessageRole, "lastMessage"},  // TODO
  };
}

QVariant UserList::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() >= users_.size()) return QVariant();

  const User& user = users_.at(index.row());

  switch (role) {
    case UsernameRole:
      return user.username();
    case StatusRole:
      return user.status();
    case AvatarRole:
      return user.avatar();
    case LastMessageRole:
      return "aller";
    default:
      qDebug() << "UserList: Undefined role.";
  }

  return QVariant();
}

int UserList::rowCount(const QModelIndex& parent) const {
  if (parent.isValid()) return 0;
  return users_.size();
}
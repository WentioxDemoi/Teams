#include "ContactList.h"

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qdebug.h>
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

void ContactList::addUser(const User &user) {
  beginInsertRows(QModelIndex(), users_.size(), users_.size());
  users_.append(user);
  endInsertRows();
}

void ContactList::removeUser(const User &user) {
  int row = users_.indexOf(user);
  if (row < 0)
    return;
  beginRemoveRows(QModelIndex(), row, row);
  users_.removeAt(row);
  endRemoveRows();
}

QHash<int, QByteArray> ContactList::roleNames() const {
  return {
      {UsernameRole, "username"},       {StatusRole, "status"},     {AvatarRole, "avatar"},
      {LastMessageRole, "lastMessage"}, {InitialsRole, "initials"}, {AvatarColorRole, "avatarColor"},
      {OnlineRole, "online"},           {UuidRole, "uuid"}, {UnreadRole, "unreadCount"},
  };
}

QVariant ContactList::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= users_.size())
    return {};
  const User &user = users_.at(index.row());

  switch (role) {
  case UsernameRole:
    return user.fullName();
  case StatusRole:
    return user.status();
  case AvatarRole:
    return user.avatar();
  case LastMessageRole:
    return user.lastMessage();
  case InitialsRole:
    return user.initials();
  case AvatarColorRole:
    return user.avatarColor();
  case OnlineRole:
    return user.online();
  case UuidRole:
    return user.uuid();
  case UnreadRole:
    return user.unreadCount();
  default:
    qDebug() << "ContactList: Undefined role.";
  }
  return {};
}

QVariantMap ContactList::get(int row) const {
  if (row < 0 || row >= users_.size())
    return {};
  return toVariantMap(users_.at(row));
}

int ContactList::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return users_.size();
}

QVariantMap ContactList::toVariantMap(const User &user) const {
  QVariantMap map;
  map["username"] = user.fullName();
  map["status"] = user.status();
  map["avatar"] = user.avatar();
  map["lastMessage"] = user.lastMessage();
  map["initials"] = user.initials();
  map["avatarColor"] = user.avatarColor();
  map["online"] = user.online();
  map["uuid"] = user.uuid();
  return map;
}

QVariantMap ContactList::findByUuid(const QString &userUuid) const {
  for (int i = 0; i < rowCount(); ++i) {
    QVariantMap u = get(i);
    if (u["uuid"].toString() == userUuid) {
      return u;
    }
  }
  return {};
}

void ContactList::updateLastMessage(const QString &uuid, const QString &message) {
  for (int row = 0; row < users_.size(); ++row) {
    if (users_[row].uuid() == uuid) {

      users_[row].setLastMessage(message);

      QModelIndex idx = index(row);
      emit dataChanged(idx, idx, {LastMessageRole});

      return;
    }
  }
}

void ContactList::setUnreadCount(const QString &uuid, int value) {
  for (int i = 0; i < users_.size(); ++i) {
    if (users_[i].uuid() == uuid) {

      users_[i].setUnreadCount(value);

      QModelIndex idx = index(i);
      emit dataChanged(idx, idx, {UnreadRole});

      return;
    }
  }
}

void ContactList::incrementUnreadCount(const QString &uuid)
{
    for (int i = 0; i < users_.size(); ++i) {
        if (users_[i].uuid() == uuid) {

            int newValue = users_[i].unreadCount() + 1;
            users_[i].setUnreadCount(newValue);

            emit dataChanged(index(i), index(i), {UnreadRole});
            return;
        }
    }
}

ContactList::ContactList(QObject *parent) : QAbstractListModel(parent) {}
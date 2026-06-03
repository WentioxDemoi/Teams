#include "UserList.h"

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qdebug.h>
#include <QtCore/qobject.h>
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
        {UsernameRole,    "username"},
        {StatusRole,      "status"},
        {AvatarRole,      "avatar"},
        {LastMessageRole, "lastMessage"},
        {InitialsRole,    "initials"},
        {AvatarColorRole, "avatarColor"},
        {OnlineRole,      "online"},
    };
}

QVariant UserList::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= users_.size()) return {};
    const User& user = users_.at(index.row());

    switch (role) {
        case UsernameRole:    return user.fullName();
        case StatusRole:      return user.status();
        case AvatarRole:      return user.avatar();
        case LastMessageRole: return user.lastMessage();
        case InitialsRole:    return user.initials();
        case AvatarColorRole: return user.avatarColor();
        case OnlineRole:      return user.online();
        default:
            qDebug() << "UserList: Undefined role.";
    }
    return {};
}

QVariantMap UserList::get(int row) const {
    if (row < 0 || row >= users_.size()) return {};
    const User& u = users_.at(row);
    return {
        {"username",    u.fullName()},
        {"initials",    u.initials()},
        {"avatarColor", u.avatarColor()},
        {"online",      u.online()},
    };
}

int UserList::rowCount(const QModelIndex& parent) const {
  if (parent.isValid()) return 0;
  return users_.size();
}

UserList::UserList(QObject *parent)
    : QAbstractListModel(parent)
{
  addUser(User("alice@example.com", "Alice", "Martin",  "En ligne", false, "", "uuid-alice", "", "OK je regarde ça ce soir 👍"));
  addUser(User("bob@example.com",   "Bob",   "Dupont",  "Absent",   false, "", "uuid-bob",   "", "Tu peux m'envoyer le fichier ?"));
  addUser(User("clara@example.com", "Clara", "Roux",    "En ligne", false, "", "uuid-clara", "", "Parfait, à demain alors !"));
  addUser(User("brice@example.com", "Brice", "Roux",    "En ligne", false, "", "uuid-brice", "", "Parfait, à jamais alors !"));
}
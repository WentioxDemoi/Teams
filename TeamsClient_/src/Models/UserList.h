#ifndef USERLIST_H
#define USERLIST_H

#include <Qlist.h>
#include <QtCore/qtmetamacros.h>
#include <arm/types.h>
#include <QAbstractListModel>
#include <QDebug>
#include <QHashFunctions>
#include <QJsonObject>
#include <QObject>
#include <QString>

#include "User.h"

class UserList : public QAbstractListModel {
  Q_OBJECT
 public:
  enum Roles {
    UsernameRole = Qt::UserRole + 1,
    StatusRole,
    AvatarRole,
    LastMessageRole,
  };

  void addUser(const User& user);
  void removeUser(const User& user);  // Deuxième temps

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

 private:
  QList<User> users_;
};

#endif
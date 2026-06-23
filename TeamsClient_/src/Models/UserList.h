#ifndef USERLIST_H
#define USERLIST_H

#include <Qlist.h>
#include <QtCore/qobject.h>
#include <QtCore/qtmetamacros.h>

#include <QAbstractListModel>
#include <QDebug>
#include <QHashFunctions>
#include <QJsonObject>
#include <QObject>
#include <QString>

#include "MessageList.h"
#include "User.h"

/**
 * @class UserList
 * @brief Modèle de liste d'utilisateurs pour QML.
 *
 * Gère un ensemble d'utilisateurs, leurs rôles de données et l'accès
 * aux informations nécessaires pour afficher un carnet de contacts.
 */
class UserList : public QAbstractListModel {
  Q_OBJECT
 public:
  enum Roles {
    UsernameRole = Qt::UserRole + 1,
    StatusRole,         // "En ligne" / "Absent"
    AvatarRole,         // URL avatar
    LastMessageRole,
    InitialsRole,       // "AM", "BD"...
    AvatarColorRole,    // couleur hex
    OnlineRole,         // bool
    UuidRole,
};

  void addUser(const User& user);
  void removeUser(const User& user);  // Deuxième temps

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  Q_INVOKABLE QVariantMap get(int row) const;

  QVariantMap toVariantMap(const User& user) const;

  explicit UserList(QObject *parent = nullptr);

 private:
  QList<User> users_;
};

#endif
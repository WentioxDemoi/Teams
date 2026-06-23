#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QList>
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
 * @class ContactList
 * @brief Modèle de liste de contacts pour QML.
 *
 * Gère un ensemble de contacts, leurs rôles de données et l'accès
 * aux informations nécessaires pour afficher un carnet de contacts.
 */
class ContactList : public QAbstractListModel {
  Q_OBJECT
public:
  enum Roles {
    UsernameRole = Qt::UserRole + 1,
    StatusRole, // "En ligne" / "Absent"
    AvatarRole, // URL avatar
    LastMessageRole,
    InitialsRole,    // "AM", "BD"...
    AvatarColorRole, // couleur hex
    OnlineRole,      // bool
    UuidRole,
    UnreadRole,
  };

  void addUser(const User &user);
  void removeUser(const User &user); // Deuxième temps
  void updateLastMessage(const QString &uuid, const QString &message);
  void setUnreadCount(const QString &uuid, int value);
  void incrementUnreadCount(const QString &uuid);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  Q_INVOKABLE QVariantMap get(int row) const;

  QVariantMap toVariantMap(const User &user) const;
  QVariantMap
  findByUuid(const QString &uuid) const; // User invalide si non trouvé

  explicit ContactList(QObject *parent = nullptr);

private:
  QList<User> users_;
};

#endif
#ifndef SEARCHRESULTS_H
#define SEARCHRESULTS_H

#include <Qlist.h>
#include <QtCore/qobject.h>
#include <QtCore/qtmetamacros.h>
#include <arm/types.h>

#include <QAbstractListModel>
#include <QDebug>
#include <QHashFunctions>
#include <QJsonObject>
#include <QObject>
#include <QString>

#include "MessageList.h"
#include "User.h"

/**
 * @class SearchResults
 * @brief Modèle de liste pour les résultats de recherche d'utilisateurs.
 *
 * Reçoit les utilisateurs renvoyés par le serveur lors d'une recherche
 * et expose les rôles nécessaires à l'affichage dans le dropdown QML.
 */
class SearchResults : public QAbstractListModel {
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
  void removeUser(const User& user);
  void setUsers(const QList<User>& users);  // remplace tout le contenu (recherche)
  void clear();                              // vide la liste (champ de recherche vidé)
  QList<User> getSearchResultsList() { return users_; }

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  Q_INVOKABLE QVariantMap get(int row) const;

  explicit SearchResults(QObject *parent = nullptr);

 private:
  QList<User> users_;
};

#endif
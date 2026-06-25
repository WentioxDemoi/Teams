#ifndef ICONTACTSERVICE_H
#define ICONTACTSERVICE_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>

#include "SessionEnum.h"

class User;

/**
 * @class IContactService
 * @brief Interface pour la gestion des contacts utilisateurs.
 *
 * Permet de charger les users depuis le backend, de sauvegarder ou supprimer
 * des contacts locaux et de notifier les vues de l'état de la connexion.
 */
class IContactService : public QObject {
  Q_OBJECT
public:
  explicit IContactService(QObject *parent = nullptr) : QObject(parent) {};
  virtual ~IContactService() = default;

  virtual void searchUsers(const QString &query) = 0;
  virtual void resolveUserByUuid(const QString &uuid) = 0;
  virtual void saveContact(const User &user) = 0;
  virtual void removeContact(const QString &uuid) = 0;
  virtual void updateLastReadAt(const QString &uuid) = 0;
  virtual void deleteAll() = 0;
  virtual void disconnectFromServer() = 0;

signals:
  void userResolved(const User &user);
  void usersSearchLoaded(const QList<User> &contacts);

  
  void contactStatusUpdated(const QString &uuid, const QString &status);
  void contactsLoaded(const QList<User> &contacts);
  void contactSaved(const User &user);
  void contactDeleted(const QString &uuid);
  void contactError(const QString &error);

  void connectionUpdate(ServerType server, bool status);
};

#endif

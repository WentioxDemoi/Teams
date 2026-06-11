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
  explicit IContactService(QObject* parent = nullptr) : QObject(parent) {};
  virtual ~IContactService() = default;

 public slots:
  virtual void loadContacts() = 0;
  virtual void saveContact(const User& user) = 0;
  virtual void deleteContact(const QString& uuid) = 0;
  virtual void disconnectFromServer() = 0;

 signals:
  void contactsLoaded(const QList<QJsonObject>& contacts);
  void contactSaved(const User& user);
  void contactDeleted(const QString& uuid);
  void contactError(const QString& error);
  void connectionUpdate(ServerType server, bool status);
};

#endif

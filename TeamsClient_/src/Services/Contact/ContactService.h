#ifndef CONTACTSERVICE_H
#define CONTACTSERVICE_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QtCore/qhashfunctions.h>

#include "../../Repositories/UserRepository.h"
#include "../Interfaces/IContactService.h"
#include "../Network/NetworkService.h"

/**
 * @class ContactService
 * @brief Service de gestion des contacts.
 *
 * Chargement des utilisateurs depuis le serveur de contacts sur le port 8084,
 * persistance locale via UserRepository et exposition des événements aux vues.
 */
class ContactService : public IContactService {
  Q_OBJECT
public:
  explicit ContactService(NetworkService *network = nullptr,
                          UserRepository *userRepo = nullptr,
                          QObject *parent = nullptr);

  void searchUsers(const QString &query) override;
  void resolveUserByUuid(const QString &uuid) override;
  void saveContact(const User &user) override;
  void removeContact(const QString &uuid) override;
  void deleteAll() override;
  void disconnectFromServer() override;



private:

  void handleServerResponse(const QJsonObject &root);
  void loadContactsFromServer();
  void loadContactsFromDatabase();
  void persistContacts(const QList<User> &users);
  QList<User> parseUsersArray(const QJsonArray &array);

  NetworkService *network_;
  UserRepository *userRepo_;
};

#endif

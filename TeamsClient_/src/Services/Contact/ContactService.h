#ifndef CONTACTSERVICE_H
#define CONTACTSERVICE_H

#include <QJsonObject>
#include <QObject>
#include <QList>

#include "../Interfaces/IContactService.h"
#include "../Network/NetworkService.h"
#include "../../Repositories/UserRepository.h"

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
  explicit ContactService(NetworkService* network = nullptr,
                          UserRepository* userRepo = nullptr,
                          QObject* parent = nullptr);

 public slots:
  void loadContacts() override;
  void saveContact(const User& user) override;
  void deleteContact(const QString& uuid) override;
  void disconnectFromServer() override;

 private slots:
  void handleServerResponse(const QJsonObject& root);

 private:
  void persistContacts(const QList<User>& users);

  NetworkService* network_;
  UserRepository* userRepo_;
};

#endif

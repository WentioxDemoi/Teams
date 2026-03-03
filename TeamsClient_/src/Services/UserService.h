#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "../Repositories/UserRepository.h"
#include "../Models/User.h"
#include "Interfaces/IUserService.h"

/**
 * @class UserService
 * @brief Service de gestion des utilisateurs.
 *
 * Fournit des méthodes pour sauvegarder, supprimer un utilisateur ou tous les utilisateurs,
 * en s'appuyant sur le UserRepository pour l'accès aux données.
 */
class UserService : public IUserService
{
    Q_OBJECT
public:
    explicit UserService(QObject* parent = nullptr);

public slots:
    void saveUser(const User& user) override;
    void deleteUser(QString uuid) override;
    void deleteAll() override;


private:
    UserRepository *repo_;
};

#endif
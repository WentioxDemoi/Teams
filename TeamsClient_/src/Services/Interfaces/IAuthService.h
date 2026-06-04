#ifndef IAUTHSERVICE_H
#define IAUTHSERVICE_H

#include "../../Models/User.h"

/**
 * @class IAuthService
 * @brief Interface pour la gestion de l'authentification des utilisateurs.
 *
 * Définit les méthodes que tout service d'authentification doit implémenter.
 * Sert notamment à faciliter les tests unitaires et à découpler la couche réseau
 * de la logique métier.
 */
class IAuthService : public QObject
{
    Q_OBJECT
public:
    explicit IAuthService(QObject *parent = nullptr) : QObject(parent) {};
    virtual ~IAuthService() = default;
    virtual void loginUser(const QString &username, const QString &password) = 0;
    virtual void registerUser(const QString &firstName, const QString &lastName, const QString &email, const QString &password) = 0;
    virtual void loginWithToken(const QString &token) = 0;
    virtual void disconnectFromServer() = 0;


signals:
    void authSuccess(const User &user);
    void authError(const QString &error);
    void errorToken(const QString& error);

public slots:

};

#endif
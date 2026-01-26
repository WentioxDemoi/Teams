#ifndef SESSIONSERVICE_H
#define SESSIONSERVICE_H

#include "../includes.h"
#include "../Models/User.h"

/**
 * @class SessionService
 * @brief Interface pour ...
 *
 * Définit les méthodes que tout service ...
 */
class SessionService : public QObject
{
    Q_OBJECT
public:
    explicit SessionService(QObject *parent = nullptr) : QObject(parent) {};
    void start();



signals:
    void SessionSuccess(const User &user);
    void SessionError(const QString &error);
    void noTokenFound();
};

#endif
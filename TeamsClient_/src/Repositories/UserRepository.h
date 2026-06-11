#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include <QSqlDatabase>

#include "../Models/User.h"

/**
 * @class UserRepository
 * @brief Répertoire d'accès aux données utilisateurs.
 *
 * Fournit des méthodes pour rechercher, insérer, mettre à jour ou supprimer des utilisateurs
 * dans la base de données, ainsi que pour vérifier la présence d'un utilisateur ou déterminer
 * si aucun utilisateur n'existe encore.
 */
class UserRepository : public QObject {
    Q_OBJECT

public:
    explicit UserRepository(QObject* parent = nullptr);

    bool save(const User& user);

    std::optional<User> findByUUID(const QString& uuid);
    QList<User> findAll();

    bool remove(const QString& uuid);

    bool removeAll();
    bool removeAllContacts();

signals:
    void error(const QString& message);

private:
    QSqlDatabase db_;
};

#endif
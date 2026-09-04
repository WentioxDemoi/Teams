#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include <QSqlDatabase>

#include "IUserRepository.h"

/**
 * @class UserRepository
 * @brief Répertoire d'accès aux données utilisateurs.
 *
 * Fournit des méthodes pour rechercher, insérer, mettre à jour ou supprimer des utilisateurs
 * dans la base de données, ainsi que pour vérifier la présence d'un utilisateur ou déterminer
 * si aucun utilisateur n'existe encore.
 */
class UserRepository : public IUserRepository {
    Q_OBJECT

public:
    explicit UserRepository(QObject* parent = nullptr);

    bool save(const User& user) override;

    std::optional<User> findByUUID(const QString& uuid);
    QList<User> findAll() override;

    bool remove(const QString& uuid);

    bool removeAll() override;
    bool removeAllContacts();
    bool updateLastReadAt(const QString &uuid, const QString &lastReadAt) override;

signals:
    void error(const QString& message);

private:
    QSqlDatabase db_;
};

#endif
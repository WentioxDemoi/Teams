#ifndef ILOCALUSERSERVICE_H
#define ILOCALUSERSERVICE_H

#include <QObject>
#include "User.h"

/**
 * @class ILocalUserService
 * @brief Interface pour la gestion du user local (l'utilisateur courant de l'application).
 *
 * Ne couvre pas la gestion des contacts (cf. IContactService).
 */
class ILocalUserService : public QObject {
  Q_OBJECT
 public:
  explicit ILocalUserService(QObject* parent = nullptr) : QObject(parent) {};
  virtual ~ILocalUserService() = default;

 public slots:
  virtual void saveLocalUser(const User& user) = 0;
  virtual void deleteAll() = 0;

 signals:
  void localUserSaved(const User& user);
};

#endif
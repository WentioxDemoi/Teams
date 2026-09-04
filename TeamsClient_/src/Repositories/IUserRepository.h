#ifndef IUSERREPOSITORY_H
#define IUSERREPOSITORY_H

#include "Models/User.h"

#include <QList>
#include <QObject>

class IUserRepository : public QObject {
  Q_OBJECT

 public:
  explicit IUserRepository(QObject* parent = nullptr) : QObject(parent) {}
  ~IUserRepository() override = default;

  virtual bool save(const User& user) = 0;
  virtual QList<User> findAll() = 0;
  virtual bool removeAll() = 0;
  virtual bool updateLastReadAt(const QString& uuid, const QString& lastReadAt) = 0;
};

#endif
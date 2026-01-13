#ifndef SERVICELOCATOR_H
#define SERVICELOCATOR_H

#include "../includes.h"

class ServiceLocator {
public:
  static ServiceLocator &instance();

  template <typename T> T *getService() {
    QString key = typeid(T).name();
    if (services_.contains(key)) {
      return static_cast<T *>(services_[key]);
    }
    return nullptr;
  }

  template <typename T> void registerService(T *service) {
    QString key = typeid(T).name();
    services_[key] = service;
  }
  void clear();

private:
  ServiceLocator() = default;
  ~ServiceLocator() { clear(); }

  ServiceLocator(const ServiceLocator &) = delete;
  ServiceLocator(ServiceLocator &&) = delete;
  ServiceLocator &operator=(const ServiceLocator &) = delete;
  ServiceLocator &operator=(ServiceLocator &&) = delete;

  QMap<QString, QObject *> services_;
};

#endif
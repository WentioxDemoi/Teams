#ifndef STATELOCATOR_H
#define STATELOCATOR_H

#include <QMap>
#include <QObject>
#include <QString>
#include <typeinfo>

/**
 * @class StateLocator
 * @brief Gestionnaire centralisé des singletons d'état de l'application.
 *
 * Permet d'enregistrer et de récupérer des instances de state par type.
 */
class StateLocator {
 public:
  static StateLocator& instance();

  template <typename T>
  T* getState() {
    QString key = typeid(T).name();
    if (states_.contains(key)) {
      return static_cast<T*>(states_[key]);
    }
    return nullptr;
  }

  template <typename T>
  void registerState(T* state) {
    QString key = typeid(T).name();
    states_[key] = state;
  }

  void clear();

  StateLocator() = default;
  ~StateLocator() { clear(); }

  StateLocator(const StateLocator&) = delete;
  StateLocator(StateLocator&&) = delete;
  StateLocator& operator=(const StateLocator&) = delete;
  StateLocator& operator=(StateLocator&&) = delete;

 private:
  QMap<QString, QObject*> states_;
};

#endif

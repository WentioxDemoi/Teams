#ifndef VIEWLOCATOR_H
#define VIEWLOCATOR_H

#include "../includes.h"

/**
 * @class ViewLocator
 * @brief Gestionnaire centralisé des views de l'application.
 *
 * Permet d'enregistrer et de récupérer des instances de views par type.
 * Utilise un singleton pour garantir un point d'accès unique et simplifier
 * l'injection de dépendances dans les différentes couches de l'application.
 */
class ViewLocator
{
public:
  static ViewLocator &instance();

  template <typename T>
  T *getView()
  {
    QString key = typeid(T).name();
    if (views_.contains(key))
    {
      return static_cast<T *>(views_[key]);
    }
    return nullptr;
  }

  template <typename T>
  void registerView(T *view)
  {
    QString key = typeid(T).name();
    views_[key] = view;
  }
  void clear();

private:
  ViewLocator() = default;
  ~ViewLocator() { clear(); }

  ViewLocator(const ViewLocator &) = delete;
  ViewLocator(ViewLocator &&) = delete;
  ViewLocator &operator=(const ViewLocator &) = delete;
  ViewLocator &operator=(ViewLocator &&) = delete;

  QMap<QString, QObject *> views_;
};

#endif
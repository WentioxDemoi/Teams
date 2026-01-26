#ifndef VIEWMODELSLOCATOR_H
#define VIEWMODELSLOCATOR_H

#include "../includes.h"

/**
 * @class ViewModelsLocator
 * @brief Gestionnaire centralisé des viewModels de l'application.
 *
 * Permet d'enregistrer et de récupérer des instances de viewModels par type.
 * Utilise un singleton pour garantir un point d'accès unique et simplifier
 * l'injection de dépendances dans les différentes couches de l'application.
 */
class ViewModelsLocator
{
public:
  static ViewModelsLocator &instance();

  template <typename T>
  T *getViewModels()
  {
    QString key = typeid(T).name();
    if (viewModels_.contains(key))
    {
      return static_cast<T *>(viewModels_[key]);
    }
    return nullptr;
  }

  template <typename T>
  void registerViewModels(T *viewModels)
  {
    QString key = typeid(T).name();
    viewModels_[key] = viewModels;
  }
  void clear();

private:
  ViewModelsLocator() = default;
  ~ViewModelsLocator() { clear(); }

  ViewModelsLocator(const ViewModelsLocator &) = delete;
  ViewModelsLocator(ViewModelsLocator &&) = delete;
  ViewModelsLocator &operator=(const ViewModelsLocator &) = delete;
  ViewModelsLocator &operator=(ViewModelsLocator &&) = delete;

  QMap<QString, QObject *> viewModels_;
};

#endif
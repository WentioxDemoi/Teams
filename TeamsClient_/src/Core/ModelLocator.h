#ifndef MODELLOCATOR_H
#define MODELLOCATOR_H

#include <QMap>
#include <QObject>
#include <QString>
#include <typeinfo>

/**
 * @class ModelLocator
 * @brief Gestionnaire centralisé des modèles de l'application.
 *
 * Permet d'enregistrer et récupérer des modèles C++ exposés au QML.
 */
class ModelLocator {
 public:
  static ModelLocator& instance();

  template <typename T>
  T* getModel() {
    QString key = typeid(T).name();

    if (models_.contains(key)) {
      return static_cast<T*>(models_[key]);
    }

    return nullptr;
  }

  template <typename T>
  void registerModel(T* model) {
    QString key = typeid(T).name();
    models_[key] = model;
  }

  void clear();

  ModelLocator() = default;
  ~ModelLocator() { clear(); }

  ModelLocator(const ModelLocator&) = delete;
  ModelLocator(ModelLocator&&) = delete;

  ModelLocator& operator=(const ModelLocator&) = delete;
  ModelLocator& operator=(ModelLocator&&) = delete;

 private:
  QMap<QString, QObject*> models_;
};

#endif
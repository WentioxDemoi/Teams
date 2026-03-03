#ifndef VIEWMODELSTOOLS_H
#define VIEWMODELSTOOLS_H

#include <QRegularExpression>
#include <QString>

/**
 * @class ViewModelsTools
 * @brief Classe utilitaire statique pour les ViewModels.
 *
 * Fournit des fonctions utilitaires, comme la validation d'email,
 * sans possibilité d'instanciation.
 */
class ViewModelsTools {
 public:
  static bool isValidEmail(const QString& email) {
    static const QRegularExpression rx(R"((^[\w\.\-]+@[A-Za-z0-9\.\-]+\.[A-Za-z]{2,}$))");
    return rx.match(email).hasMatch();
  }

  ViewModelsTools() = delete;
};

#endif
#ifndef LOADINGVIEW_H
#define LOADINGVIEW_H

#include <QLabel>
#include <QProgressBar>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @class LoadingView
 * @brief Vue d'affichage d'un écran de chargement.
 *
 * Affiche un message et une barre de progression pour indiquer à l'utilisateur
 * que l'application est en cours de traitement ou de chargement.
 */
class LoadingView : public QWidget {
  Q_OBJECT

 public:
  explicit LoadingView(const QString& message, QWidget* parent = nullptr);

  void setMessage(const QString& message);

 private:
  QLabel* m_label;
  QProgressBar* m_progress;
};

#endif
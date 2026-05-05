#ifndef TOPBAR_H
#define TOPBAR_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtWidgets/qframe.h>
#include <QtWidgets/qlineedit.h>
#include <QHBoxLayout>

#include "User.h"

/**
 * @class Topbar
 * @brief Barre supérieure de l'application
 *
 * Ce widget fournit :
 * - un accès au profil utilisateur
 * - un champ de recherche global
 *
 * Émet un signal lors d'une recherche utilisateur et
 * affiche les résultats correspondants.
 */
class Topbar : public QFrame {
  Q_OBJECT

 public:
  explicit Topbar(QWidget* parent = nullptr);

 signals:

  /**
  * @brief Signal émis lorsqu'une recherche est lancée
  */
  void sendTBSearch(const QString& prompt);


 public slots:

  /**
  * @brief Affiche les résultats de recherche utilisateur
  */
  void displayTBResults(const QVector<User>& users);



 private:
 void initProfilButton();
 
  QHBoxLayout* mainLayout_;
  QPushButton* profilButton_;
  QLineEdit *searchInput_;
};

#endif
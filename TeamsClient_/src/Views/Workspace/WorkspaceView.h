#ifndef WORKSPACEVIEW_H
#define WORKSPACEVIEW_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @class WorkspaceView
 * @brief Vue principale de l’espace de travail.
 *
 * Gère l’interface utilisateur liée au workspace, notamment l’affichage
 * des éléments principaux.
 */
class WorkspaceView : public QWidget {
  Q_OBJECT

 public:
  explicit WorkspaceView(QWidget* parent = nullptr);
  void startSender();
  void startReceiver();

 signals:
  void startVisio();
  void OnP2PChange(bool inProgress);
  void initP2P();

 public slots:

 private:
  QLabel* titleLabel_;
  QWidget* contentWidget_;
  QVBoxLayout* mainLayout_;
  QPushButton* visioButton_;
  QPushButton* receiveButton_;

  // // ========= VisioConf POC =========
  // Visio *visio_;
};

#endif
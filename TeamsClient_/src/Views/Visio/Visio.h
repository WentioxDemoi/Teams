#ifndef VISIO_H
#define VISIO_H

#include <QMainWindow>
#include <QVBoxLayout>

#include "Local/LocalVideo.h"
#include "Remote/RemoteVideo.h"

/**
 * @class Visio
 * @brief Fenêtre de visioconférence.
 *
 * Gère l'affichage et la coordination des flux vidéo locaux et distants,
 * ainsi que le démarrage des sessions d'envoi et de réception.
 */
class Visio : public QMainWindow {
  Q_OBJECT

 public:
  explicit Visio(QWidget* parent = nullptr);
  void startSender();
  void startReceiver();

 protected:
  void resizeEvent(QResizeEvent* event) override;
  void closeEvent(QCloseEvent* event) override;

 public slots:

 signals:
  void OnP2PChange(bool inProgress);

 private:
  void start();

  void updateLocalVideoPosition();

 private:
  QWidget* mainWidget = nullptr;
  QVBoxLayout* layout = nullptr;

  RemoteVideo* remoteVideo = nullptr;
  LocalVideo* localVideo = nullptr;
};

#endif
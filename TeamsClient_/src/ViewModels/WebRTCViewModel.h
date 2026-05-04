#ifndef WEBRTCVIEWMODEL_H
#define WEBRTCVIEWMODEL_H

#include "../Services/P2P/WebRTCService.h"
#include "ViewModelsTools.h"

/**
 * @class WebRTCViewModel
 * @brief ViewModel pour la gestion WebRTC.
 *
 * Coordonne l'initialisation et le suivi des connexions P2P via WebRTC,
 * en relayant les changements d'état et en gérant l'enregistrement auprès
 * du serveur pour l'identifiant unique de l'utilisateur.
 */
class WebRTCViewModel : public QObject {
  Q_OBJECT

 public:
  explicit WebRTCViewModel(WebRTCService* webRTCService = nullptr, QObject* parent = nullptr);
  void start();

 public slots:
  void initP2P();

 signals:
  void onP2PChange(bool inProgress);
  void registerWithServer4WebRTC(QString UUID);

 private:
  WebRTCService* webRTCService_;
};

#endif
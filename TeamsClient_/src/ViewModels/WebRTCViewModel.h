#ifndef WEBRTCVIEWMODEL_H
#define WEBRTCVIEWMODEL_H

#include "../Core/ServiceLocator.h"
#include "../Models/User.h"
#include "../includes.h"
#include "ViewModelsTools.h"
#include "../Services/WebRTCService.h"
#include "../Services/Interfaces/ISessionService.h"

/**
 * @class WebRTCViewModel
 * @brief ViewModel ....
 */
class WebRTCViewModel : public QObject
{
  Q_OBJECT

public:
  explicit WebRTCViewModel(WebRTCService *webRTCService = nullptr, QObject *parent = nullptr); // Aller voir dans le CPP l'implémentation
  void start();

public slots:
  void initP2P();
  
signals:
    void onP2PChange(bool inProgress);
    void registerWithServer4WebRTC(QString UUID);
  

private:

  WebRTCService *webRTCService_;
};

#endif
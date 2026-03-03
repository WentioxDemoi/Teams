#ifndef WEBRTCVIEWMODEL_H
#define WEBRTCVIEWMODEL_H

#include "ViewModelsTools.h"
#include "../Services/P2P/WebRTCService.h"

/**
 * @class WebRTCViewModel
 * @brief ViewModel ....
 */
class WebRTCViewModel : public QObject
{
  Q_OBJECT

public:
  explicit WebRTCViewModel(WebRTCService *webRTCService = nullptr, QObject *parent = nullptr);
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
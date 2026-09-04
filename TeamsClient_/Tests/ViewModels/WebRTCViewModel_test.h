#ifndef WEBRTC_VIEW_MODEL_TEST_H
#define WEBRTC_VIEW_MODEL_TEST_H

#include <QObject>

class WebRTCViewModelTest : public QObject {
  Q_OBJECT
 private slots:
  void startsWithExpectedState();
  void toggleMicDelegatesAndEmits();
  void hangupDelegatesToWebRtcAndChat();
  void remoteConnectionStateUpdatesProperty();
  void remoteCameraStateUpdatesProperty();
  void closeWindowWithoutWindowIsSafe();
  void setVideoSinksAcceptsAndClearsSinks();
  void localCaptureFrameReachesLocalSink();
  void disablingRemoteCameraClearsRemoteSinkFrame();
  void connectionStateSupportsBothValues();
  void hangupCallsBothServicesExactlyOnce();
};

#endif
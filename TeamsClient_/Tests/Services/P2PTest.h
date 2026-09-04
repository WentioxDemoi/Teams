#ifndef P2P_TEST_H
#define P2P_TEST_H

#include <QObject>

class P2PTest : public QObject {
  Q_OBJECT

 private slots:
  void sourcesAreSingletonAndExposeBothVideoSources();
  void localVideoSourceReportsLiveLocalState();
  void localVideoSourcePushesI420Frames();
  void remoteVideoSourceForwardsConvertedFrames();
  void remoteVideoSourceWithoutCallbackIsSafe();
};

#endif
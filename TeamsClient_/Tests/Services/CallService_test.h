#ifndef CALL_SERVICE_TEST_H
#define CALL_SERVICE_TEST_H
#include <QObject>
class CallServiceTest : public QObject {
  Q_OBJECT
 private slots:
  void startCallSendsRequestAndOpensWindow();
  void secondCallIsRejected();
  void signalingResponsesAreForwarded();
  void incomingOfferCanBeAccepted();
  void hangupSendsRequestAndClosesWindow();
  void rejectCallSendsRequest();
  void cameraChangeSendsRequest();
  void disconnectsNetwork();
  void invalidIncomingOfferIsIgnored();
  void requestAckStartsWebRtcOffer();
  void localWebRtcCallbacksSendSignalingMessages();
  void remoteAnswerAndIceReachWebRtc();
  void remoteCallEventsCloseOrCancelCall();
  void acceptWithoutOfferEmitsError();
  void offerDuringCallSendsBusyResponse();
  void cameraCanBeDisabled();
  void remoteStateSignalsAreForwarded();
  void networkErrorsAreForwarded();
  void invalidServerMessagesAreIgnored();
  void timeoutCancelsCall();
  void hangupOutsideCallOnlyStopsWebRtc();
};
#endif
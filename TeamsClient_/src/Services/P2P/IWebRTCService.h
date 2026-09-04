#ifndef IWEBRTCSERVICE_H
#define IWEBRTCSERVICE_H

#include <QObject>

#include <functional>
#include <string>

class IWebRTCService : public QObject {
  Q_OBJECT

 public:
  explicit IWebRTCService(QObject* parent = nullptr) : QObject(parent) {}
  ~IWebRTCService() override = default;

  virtual void setCallBacks(std::function<void(const std::string&)> onLocalOffer,
                            std::function<void(const std::string&)> onLocalAnswer,
                            std::function<void(const std::string&, const std::string&, int)> onLocalIce,
                            std::function<void(bool)> isContactConnectedChanged) = 0;
  virtual void startCall() = 0;
  virtual void acceptCall() = 0;
  virtual void hangup() = 0;
  virtual void setMicEnabled(bool enabled) = 0;
  virtual void onRemoteOffer(QString sdp) = 0;
  virtual void onRemoteAnswer(QString sdp) = 0;
  virtual void onRemoteIce(QString candidate, QString mid, int index) = 0;
};

#endif
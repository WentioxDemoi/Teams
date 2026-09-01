#ifndef IWEBRTCSERVICE_H
#define IWEBRTCSERVICE_H

#include <optional>
#include <string>

/**
 * @class IWebRTCService
 * @brief Contrat métier pour la logique de signalisation WebRTC.
 */
class IWebRTCService {
 public:
  virtual ~IWebRTCService() = default;

  virtual std::optional<std::string> requestCall(const std::string &callerUuid, const std::string &targetUuid) = 0;
  virtual bool acceptCall(const std::string &calleeUuid, const std::string &targetUuid) = 0;
  virtual bool rejectCall(const std::string &calleeUuid, const std::string &targetUuid) = 0;
  virtual bool cancelCall(const std::string &callerUuid, const std::string &targetUuid) = 0;
  virtual bool hangupCall(const std::string &senderUuid, const std::string &targetUuid) = 0;
  virtual bool relaySignaling(const std::string &senderUuid, const std::string &targetUuid, const std::string &payload) = 0;
  virtual bool cameraEnabledChange(const std::string &senderUuid, const std::string &targetUuid, const std::string &payload) = 0;
};

#endif

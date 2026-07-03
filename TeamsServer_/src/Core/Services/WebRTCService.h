#ifndef WEBRTCSERVICE_H
#define WEBRTCSERVICE_H

#include "../../Utils/Crypto.h"
#include "../Repositories/UserRepository.h"
#include "../Core/Registeries/WebRTCRegistry.h"

#include <optional>
#include <string>

/**
 * @class WebRTCService
 * @brief Logique métier de la signalisation WebRTC : gestion de l'état d'un appel
 *        (request/accept/reject/cancel/hangup) et relai des messages SDP/ICE
 *        entre les deux pairs via le WebRTCRegistry.
 */
class WebRTCService {
public:
  WebRTCService(std::shared_ptr<WebRTCRegistry> webRTCRegistry)
      : config_(Config::instance()), webRTCRegistry_(webRTCRegistry) {};
  virtual ~WebRTCService() = default;

  std::optional<std::string> requestCall(const std::string &callerUuid, const std::string &targetUuid);

  bool acceptCall(const std::string &calleeUuid, const std::string &targetUuid);

  bool rejectCall(const std::string &calleeUuid, const std::string &targetUuid);

  bool cancelCall(const std::string &callerUuid, const std::string &targetUuid);

  bool hangupCall(const std::string &senderUuid, const std::string &targetUuid);

  bool relaySignaling(const std::string &senderUuid, const std::string &targetUuid, const std::string &payload);

  bool cameraEnabledChange(const std::string &senderUuid, const std::string &targetUuid, const std::string &payload);


private:
  bool relayToTarget(const std::string &senderUuid, const std::string &targetUuid, std::string payload);

  Config &config_;
  std::shared_ptr<WebRTCRegistry> webRTCRegistry_;
};

#endif
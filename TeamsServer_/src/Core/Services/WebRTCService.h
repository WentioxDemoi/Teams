#ifndef WEBRTCSERVICE_H
#define WEBRTCSERVICE_H

#include <optional>
#include <string>

#include "../../Utils/Crypto.h"
#include "../Core/Registeries/WebRTCRegistry.h"
#include "../Repositories/UserRepository.h"
#include "IWebRTCService.h"

/**
 * @class WebRTCService
 * @brief Logique métier de la signalisation WebRTC : gestion de l'état d'un appel
 *        (request/accept/reject/cancel/hangup) et relai des messages SDP/ICE
 *        entre les deux pairs via le WebRTCRegistry.
 */
class WebRTCService : public IWebRTCService {
 public:
  WebRTCService(std::shared_ptr<IWebRTCRegistry> webRTCRegistry)
      : config_(Config::instance()), webRTCRegistry_(webRTCRegistry) {};
  ~WebRTCService() override = default;

  std::optional<std::string> requestCall(const std::string& callerUuid,
                                         const std::string& targetUuid) override;

  bool acceptCall(const std::string& calleeUuid, const std::string& targetUuid) override;

  bool rejectCall(const std::string& calleeUuid, const std::string& targetUuid) override;

  bool cancelCall(const std::string& callerUuid, const std::string& targetUuid) override;

  bool hangupCall(const std::string& senderUuid, const std::string& targetUuid) override;

  bool relaySignaling(const std::string& senderUuid, const std::string& targetUuid,
                      const std::string& payload) override;

  bool cameraEnabledChange(const std::string& senderUuid, const std::string& targetUuid,
                           const std::string& payload) override;

 private:
  bool relayToTarget(const std::string& senderUuid, const std::string& targetUuid,
                     std::string payload);

  Config& config_;
  std::shared_ptr<IWebRTCRegistry> webRTCRegistry_;
};

#endif
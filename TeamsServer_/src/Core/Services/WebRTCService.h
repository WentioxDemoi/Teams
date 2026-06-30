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

  // --- Gestion d'état d'appel ---
  // Retourne le payload à répondre AU CALLER (ack ou callee_offline/busy).
  std::optional<std::string> requestCall(const std::string &callerUuid, const std::string &targetUuid);

  // Pas de réponse directe nécessaire pour l'instant (le callee attendra l'answer
  // généré par sa propre négociation WebRTC côté client).
  void acceptCall(const std::string &calleeUuid, const std::string &targetUuid);

  // Relaye le refus au caller. Retourne false si le caller n'est plus connecté.
  bool rejectCall(const std::string &calleeUuid, const std::string &targetUuid);

  // Relaye l'annulation au callee. Retourne false si le callee n'est plus connecté.
  bool cancelCall(const std::string &callerUuid, const std::string &targetUuid);

  // Relaye le hangup à l'autre pair. Retourne false s'il n'est plus connecté.
  bool hangupCall(const std::string &senderUuid, const std::string &targetUuid);

  // --- Relai pur des messages de signaling SDP/ICE (offer / answer / ice) ---
  bool relaySignaling(const std::string &senderUuid, const std::string &targetUuid, const std::string &payload);

private:
  // Injecte senderUuid dans le payload et le transmet au destinataire via le registre.
  bool relayToTarget(const std::string &senderUuid, const std::string &targetUuid, std::string payload);

  Config &config_;
  std::shared_ptr<WebRTCRegistry> webRTCRegistry_;
};

#endif
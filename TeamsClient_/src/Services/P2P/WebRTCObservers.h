#include <jsep.h>
#include <set_local_description_observer_interface.h>
#include <set_remote_description_observer_interface.h>

#include <QDebug>

#include "../../webrtc_includes.h"

/**
 * @class CreateSdpObserver
 * @brief Observateur pour la création de SDP.
 *
 * Reçoit la session description générée et déclenche un callback avec le SDP sous forme de chaîne.
 * Gère également les erreurs de création de session.
 */

class CreateSdpObserver : public webrtc::CreateSessionDescriptionObserver {
 public:
  using Callback = std::function<void(const std::string&)>;
  explicit CreateSdpObserver(Callback cb) : cb_(std::move(cb)) {}

  void OnSuccess(webrtc::SessionDescriptionInterface* desc) override {
    std::string sdp;
    desc->ToString(&sdp);
    if (cb_) cb_(sdp);
  }
  void OnFailure(webrtc::RTCError error) override {
    RTC_LOG(LS_ERROR) << "[CreateSdp] " << error.message();
  }

  WEBRTC_REF_COUNT_IMPL
 private:
  Callback cb_;
};

/**
 * @class SetLocalSdpObserver
 * @brief Observateur pour l'enregistrement de la SDP locale.
 *
 * Surveille la finalisation de la mise en place de la description locale et journalise
 * les succès ou les erreurs correspondantes.
 */
class SetLocalSdpObserver : public webrtc::SetLocalDescriptionObserverInterface {
 public:
  void OnSetLocalDescriptionComplete(webrtc::RTCError error) override {
    if (!error.ok())
      RTC_LOG(LS_ERROR) << "[SetLocal] " << error.message();
    else
      qDebug() << "Local Description registered";
  }
  WEBRTC_REF_COUNT_IMPL
};

/**
 * @class SetRemoteSdpObserver
 * @brief Observateur pour l'enregistrement de la SDP distante.
 *
 * Surveille la finalisation de la mise en place de la description distante et journalise
 * les succès ou les erreurs correspondantes.
 */
class SetRemoteSdpObserver : public webrtc::SetRemoteDescriptionObserverInterface {
 public:
  void OnSetRemoteDescriptionComplete(webrtc::RTCError error) override {
    if (!error.ok())
      RTC_LOG(LS_ERROR) << "[SetRemote] " << error.message();
    else
      qDebug() << "Remote Description registered";
  }
  WEBRTC_REF_COUNT_IMPL
};
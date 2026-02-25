#include "../includes.h"
#include "../webrtc_includes.h"

// ─── CreateOffer / CreateAnswer ─────────────────────────────────────────────
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

// ─── SetLocalDescription ────────────────────────────────────────────────────
class SetLocalSdpObserver : public webrtc::SetLocalDescriptionObserverInterface {
public:
    void OnSetLocalDescriptionComplete(webrtc::RTCError error) override {
        if (!error.ok()) RTC_LOG(LS_ERROR) << "[SetLocal] " << error.message();
    }
    WEBRTC_REF_COUNT_IMPL
};

// ─── SetRemoteDescription ───────────────────────────────────────────────────
class SetRemoteSdpObserver : public webrtc::SetRemoteDescriptionObserverInterface {
public:
    void OnSetRemoteDescriptionComplete(webrtc::RTCError error) override {
        if (!error.ok()) RTC_LOG(LS_ERROR) << "[SetRemote] " << error.message();
    }
    WEBRTC_REF_COUNT_IMPL
};
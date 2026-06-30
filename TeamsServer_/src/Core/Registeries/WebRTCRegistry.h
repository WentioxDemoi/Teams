#ifndef WEBRTC_REGISTRY_H
#define WEBRTC_REGISTRY_H
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class WebRTCSession;

class WebRTCRegistry {
public:
    void register_session(const std::string& user_uuid,
                          std::shared_ptr<WebRTCSession> session);
    void unregister_session(const std::string& user_uuid);
    bool sendMessage(const std::string& user_uuid, const std::string& payload);
    bool isConnected(const std::string& user_uuid) const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<WebRTCSession>> webRTCSessions_;
};
#endif
#ifndef WEBRTC_REGISTRY_H
#define WEBRTC_REGISTRY_H
#include <mutex>
#include <unordered_map>

#include "IWebRTCRegistry.h"

class WebRTCSession;

class WebRTCRegistry : public IWebRTCRegistry {
 public:
  void register_session(const std::string& user_uuid,
                        std::shared_ptr<WebRTCSession> session) override;
  void unregister_session(const std::string& user_uuid) override;
  bool sendMessage(const std::string& user_uuid, const std::string& payload) override;
  bool isConnected(const std::string& user_uuid) const override;

 private:
  mutable std::mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<WebRTCSession>> webRTCSessions_;
};
#endif
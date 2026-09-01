#ifndef IWEBRTC_REGISTRY_H
#define IWEBRTC_REGISTRY_H

#include <memory>
#include <string>

class WebRTCSession;

class IWebRTCRegistry {
public:
  virtual ~IWebRTCRegistry() = default;

  virtual void register_session(const std::string &user_uuid,
                                 std::shared_ptr<WebRTCSession> session) = 0;

  virtual void unregister_session(const std::string &user_uuid) = 0;

  virtual bool sendMessage(const std::string &user_uuid, const std::string &payload) = 0;

  virtual bool isConnected(const std::string &user_uuid) const = 0;
};

#endif
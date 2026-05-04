#include "WebRTCRegistry.h"
#include "../../Network/Session/WebRTCSession.h"


void WebRTCRegistry::register_session(const std::string& uuid,
                                      std::shared_ptr<WebRTCSession> session) {
  std::lock_guard<std::mutex> lock(mutex_);
  sessions_[uuid] = session;
}

void WebRTCRegistry::unregister_session(const std::string& uuid) {
  std::lock_guard<std::mutex> lock(mutex_);
  sessions_.erase(uuid);
}

std::shared_ptr<WebRTCSession> WebRTCRegistry::find(const std::string& uuid) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = sessions_.find(uuid);
  if (it != sessions_.end()) return it->second.lock();  // weak_ptr
  return nullptr;
}

#include "Registeries/IWebRTCRegistry.h"
#include <gmock/gmock.h>

class MockWebRTCRegistry : public IWebRTCRegistry {
public:
  MOCK_METHOD(void, register_session,
              (const std::string &user_uuid, std::shared_ptr<WebRTCSession> session), (override));
  MOCK_METHOD(void, unregister_session, (const std::string &user_uuid), (override));
  MOCK_METHOD(bool, sendMessage, (const std::string &user_uuid, const std::string &payload), (override));
  MOCK_METHOD(bool, isConnected, (const std::string &user_uuid), (const, override));
};
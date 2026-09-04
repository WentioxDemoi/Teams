#include <gmock/gmock.h>

#include "Registeries/IMessageSessionRegistry.h"

class MockMessageSessionRegistry : public IMessageSessionRegistry {
 public:
  MOCK_METHOD(void, registerMessageSession,
              (const std::string& user_uuid, std::shared_ptr<MessageSession> session), (override));
  MOCK_METHOD(void, unregisterMessageSession, (const std::string& user_uuid), (override));
  MOCK_METHOD(bool, sendMessage, (const std::string& user_uuid, const std::string& payload),
              (override));
  MOCK_METHOD(bool, hasMessageSession, (const std::string& user_uuid), (const, override));
};
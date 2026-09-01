#include "Core/Registeries/ContactSessionRegistry.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockContactSessionRegistry : public IContactSessionRegistry {
public:
	MOCK_METHOD(void, registerContactSession,
							(const std::string &user_uuid, std::shared_ptr<ContactSession> session),
							(override));
	MOCK_METHOD(void, unregisterContactSession,
							(const std::string &user_uuid),
							(override));
	MOCK_METHOD(bool, sendMessage,
							(const std::string &user_uuid, const std::string &payload),
							(override));
	MOCK_METHOD(bool, hasContactSession,
							(const std::string &user_uuid),
							(const, override));
};
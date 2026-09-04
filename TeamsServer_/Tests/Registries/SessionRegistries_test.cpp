#include <gtest/gtest.h>

#include "Core/Registeries/ContactSessionRegistry.h"
#include "Core/Registeries/MessageSessionRegistry.h"
#include "Core/Registeries/WebRTCRegistry.h"

TEST(ContactSessionRegistryTest, MissingSessionReturnsFalseAndCanBeRegisteredThenRemoved) {
  ContactSessionRegistry registry;

  EXPECT_FALSE(registry.hasContactSession("u1"));
  EXPECT_FALSE(registry.sendMessage("u1", "payload"));

  registry.registerContactSession("u1", nullptr);
  EXPECT_TRUE(registry.hasContactSession("u1"));
  registry.unregisterContactSession("u1");
  EXPECT_FALSE(registry.hasContactSession("u1"));
}

TEST(MessageSessionRegistryTest, MissingSessionReturnsFalseAndCanBeRegisteredThenRemoved) {
  MessageSessionRegistry registry;

  EXPECT_FALSE(registry.hasMessageSession("u1"));
  EXPECT_FALSE(registry.sendMessage("u1", "payload"));

  registry.registerMessageSession("u1", nullptr);
  EXPECT_TRUE(registry.hasMessageSession("u1"));
  registry.unregisterMessageSession("u1");
  EXPECT_FALSE(registry.hasMessageSession("u1"));
}

TEST(WebRTCRegistryTest, MissingSessionReturnsFalseAndCanBeRegisteredThenRemoved) {
  WebRTCRegistry registry;

  EXPECT_FALSE(registry.isConnected("u1"));
  EXPECT_FALSE(registry.sendMessage("u1", "payload"));

  registry.register_session("u1", nullptr);
  EXPECT_TRUE(registry.isConnected("u1"));
  registry.unregister_session("u1");
  EXPECT_FALSE(registry.isConnected("u1"));
}
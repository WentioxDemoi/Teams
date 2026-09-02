#include "Core/Services/WebRTCService.h"

#include <gtest/gtest.h>

#include "../Mocks/WebRTCRegistryMock.h"

using ::testing::_;
using ::testing::HasSubstr;
using ::testing::Return;

namespace {

std::unique_ptr<WebRTCService> makeService(std::shared_ptr<MockWebRTCRegistry>& registry) {
  registry = std::make_shared<MockWebRTCRegistry>();
  return std::make_unique<WebRTCService>(registry);
}

}  // namespace

// ---------------------------------------------------------------------------
// requestCall
// ---------------------------------------------------------------------------

TEST(WebRTCServiceTest, RequestCall_TargetOffline_ReturnsCalleeOffline) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry, isConnected("u2")).WillOnce(Return(false));

  auto res = svc->requestCall("u1", "u2");
  ASSERT_TRUE(res.has_value());
  EXPECT_NE(res->find("callee_offline"), std::string::npos);
}

TEST(WebRTCServiceTest, RequestCall_TargetOnline_ReturnsAck) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry, isConnected("u2")).WillOnce(Return(true));

  auto res = svc->requestCall("u1", "u2");
  ASSERT_TRUE(res.has_value());
  EXPECT_NE(res->find("call_request_ack"), std::string::npos);
}

// ---------------------------------------------------------------------------
// acceptCall / rejectCall / cancelCall / hangupCall
// ---------------------------------------------------------------------------

TEST(WebRTCServiceTest, AcceptCall_RelaysToTargetWithSenderUuidAndCorrectType) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry,
              sendMessage("u2", ::testing::AllOf(HasSubstr("call_accept"), HasSubstr("u1"))))
      .WillOnce(Return(true));

  EXPECT_TRUE(svc->acceptCall("u1", "u2"));
}

TEST(WebRTCServiceTest, AcceptCall_RegistryFails_ReturnsFalse) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry, sendMessage("u2", _)).WillOnce(Return(false));

  EXPECT_FALSE(svc->acceptCall("u1", "u2"));
}

TEST(WebRTCServiceTest, RejectCall_RelaysCorrectType) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry, sendMessage("u2", HasSubstr("call_reject"))).WillOnce(Return(true));

  EXPECT_TRUE(svc->rejectCall("u1", "u2"));
}

TEST(WebRTCServiceTest, CancelCall_RelaysCorrectType) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry, sendMessage("u2", HasSubstr("call_cancel"))).WillOnce(Return(true));

  EXPECT_TRUE(svc->cancelCall("u1", "u2"));
}

TEST(WebRTCServiceTest, HangupCall_RelaysCorrectType) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry, sendMessage("u2", HasSubstr("call_hangup"))).WillOnce(Return(true));

  EXPECT_TRUE(svc->hangupCall("u1", "u2"));
}

// ---------------------------------------------------------------------------
// relaySignaling / cameraEnabledChange
// ---------------------------------------------------------------------------

TEST(WebRTCServiceTest, RelaySignaling_ForwardsPayloadWithSenderUuidInjected) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry,
              sendMessage("u2", ::testing::AllOf(HasSubstr("sdp_offer_data"), HasSubstr("u1"))))
      .WillOnce(Return(true));

  EXPECT_TRUE(svc->relaySignaling("u1", "u2", R"({"type":"offer","sdp":"sdp_offer_data"})"));
}

TEST(WebRTCServiceTest, RelaySignaling_RegistryFails_ReturnsFalse) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry, sendMessage("u2", _)).WillOnce(Return(false));

  EXPECT_FALSE(svc->relaySignaling("u1", "u2", R"({"type":"offer","sdp":"x"})"));
}

TEST(WebRTCServiceTest, CameraEnabledChange_ForwardsPayloadWithSenderUuidInjected) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry,
              sendMessage("u2", ::testing::AllOf(HasSubstr("camera_enabled"), HasSubstr("u1"))))
      .WillOnce(Return(true));

  EXPECT_TRUE(
      svc->cameraEnabledChange("u1", "u2", R"({"type":"camera_enabled","enabled":"true"})"));
}

TEST(WebRTCServiceTest, CameraEnabledChange_RegistryFails_ReturnsFalse) {
  std::shared_ptr<MockWebRTCRegistry> registry;
  auto svc = makeService(registry);

  EXPECT_CALL(*registry, sendMessage("u2", _)).WillOnce(Return(false));

  EXPECT_FALSE(
      svc->cameraEnabledChange("u1", "u2", R"({"type":"camera_enabled","enabled":"false"})"));
}
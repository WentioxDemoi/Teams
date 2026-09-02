#include "Handlers/WebRTCHandler.h"
#include "../Mocks/WebRTCServiceMock.h"
#include <gtest/gtest.h>
#include <future>

using ::testing::_;
using ::testing::HasSubstr;
using ::testing::Return;

namespace {
std::string responseFrom(std::function<void(ResponseCallback)> invoke) {
  std::promise<std::string> promise;
  auto future = promise.get_future();
  invoke([&promise](std::string response) { promise.set_value(std::move(response)); });
  EXPECT_EQ(future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
  return future.get();
}
void waitForCall(std::function<void(std::promise<void> &)> invoke) {
  std::promise<void> promise;
  auto future = promise.get_future();
  invoke(promise);
  EXPECT_EQ(future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
}
}

TEST(WebRTCHandlerTest, RequestCallPassesSenderAndTarget) {
  auto service = std::make_unique<MockWebRTCService>();
  auto *serviceMock = service.get();
  EXPECT_CALL(*serviceMock, requestCall("u1", "u2"))
      .WillOnce(Return(std::string(R"({"type":"call_request_ack"})")));
  WebRTCHandler handler(std::move(service));

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"call_request","targetUuid":"u2"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("call_request_ack"));
}

TEST(WebRTCHandlerTest, RoutesCallStateAndSignalingOperations) {
  auto service = std::make_unique<MockWebRTCService>();
  auto *serviceMock = service.get();
  WebRTCHandler handler(std::move(service));

  waitForCall([&](std::promise<void> &promise) {
    EXPECT_CALL(*serviceMock, acceptCall("u1", "u2"))
        .WillOnce([&promise](const auto &, const auto &) {
          promise.set_value();
          return true;
        });
    handler.handle_type("u1", R"({"type":"call_accept","targetUuid":"u2"})", nullptr);
  });
  waitForCall([&](std::promise<void> &promise) {
    EXPECT_CALL(*serviceMock, relaySignaling("u1", "u2", HasSubstr("sdp")))
        .WillOnce([&promise](const auto &, const auto &, const auto &) {
          promise.set_value();
          return true;
        });
    handler.handle_type("u1", R"({"type":"offer","targetUuid":"u2","sdp":"sdp"})", nullptr);
  });
}

TEST(WebRTCHandlerTest, RoutesAllCallStateOperations) {
  auto service = std::make_unique<MockWebRTCService>();
  auto *serviceMock = service.get();
  WebRTCHandler handler(std::move(service));

  waitForCall([&](std::promise<void> &promise) {
    EXPECT_CALL(*serviceMock, rejectCall("u1", "u2"))
        .WillOnce([&promise](const auto &, const auto &) { promise.set_value(); return true; });
    handler.handle_type("u1", R"({"type":"call_reject","targetUuid":"u2"})", nullptr);
  });
  waitForCall([&](std::promise<void> &promise) {
    EXPECT_CALL(*serviceMock, cancelCall("u1", "u2"))
        .WillOnce([&promise](const auto &, const auto &) { promise.set_value(); return true; });
    handler.handle_type("u1", R"({"type":"call_cancel","targetUuid":"u2"})", nullptr);
  });
  waitForCall([&](std::promise<void> &promise) {
    EXPECT_CALL(*serviceMock, hangupCall("u1", "u2"))
        .WillOnce([&promise](const auto &, const auto &) { promise.set_value(); return true; });
    handler.handle_type("u1", R"({"type":"call_hangup","targetUuid":"u2"})", nullptr);
  });
  waitForCall([&](std::promise<void> &promise) {
    EXPECT_CALL(*serviceMock, cameraEnabledChange("u1", "u2", HasSubstr("enabled")))
        .WillOnce([&promise](const auto &, const auto &, const auto &) { promise.set_value(); return true; });
    handler.handle_type("u1", R"({"type":"camera_enabled_change","targetUuid":"u2","enabled":"true"})", nullptr);
  });
}

TEST(WebRTCHandlerTest, RequestCallWithoutResponseDoesNotInvokeCallback) {
  auto service = std::make_unique<MockWebRTCService>();
  auto *serviceMock = service.get();
  WebRTCHandler handler(std::move(service));

  std::promise<void> servicePromise;
  auto serviceFuture = servicePromise.get_future();
  EXPECT_CALL(*serviceMock, requestCall("u1", "u2"))
      .WillOnce([&servicePromise](const auto &, const auto &) {
        servicePromise.set_value();
        return std::optional<std::string>{};
      });
  bool callbackCalled = false;
  handler.handle_type("u1", R"({"type":"call_request","targetUuid":"u2"})",
                      [&callbackCalled](std::string) { callbackCalled = true; });

  EXPECT_EQ(serviceFuture.wait_for(std::chrono::seconds(2)), std::future_status::ready);
  EXPECT_FALSE(callbackCalled);
}

TEST(WebRTCHandlerTest, UnknownOrEmptyTypeDoesNotCallService) {
  auto service = std::make_unique<MockWebRTCService>();
  auto *serviceMock = service.get();
  EXPECT_CALL(*serviceMock, requestCall(_, _)).Times(0);
  WebRTCHandler handler(std::move(service));

  handler.handle_type("u1", "{}", nullptr);
  handler.handle_type("u1", R"({"type":"unknown"})", nullptr);
}
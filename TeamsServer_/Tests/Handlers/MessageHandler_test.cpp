#include "Handlers/MessageHandler.h"

#include <gtest/gtest.h>

#include <future>

#include "../Mocks/MessageServiceMock.h"

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
}  // namespace

TEST(MessageHandlerTest, SendMessageInjectsSenderUuid) {
  auto service = std::make_unique<MockMessageService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, sendMessage(HasSubstr(R"("senderUuid":"u1")")))
      .WillOnce(Return(std::string(R"({"type":"message_sent"})")));
  MessageHandler handler(std::move(service));

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"send_message","content":"hello"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("message_sent"));
}

TEST(MessageHandlerTest, LoadConversationsReturnsFallbackOnFailure) {
  auto service = std::make_unique<MockMessageService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, loadConversations("u1")).WillOnce(Return(std::nullopt));
  MessageHandler handler(std::move(service));

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"load_conversations"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("conversation_response"));
}

TEST(MessageHandlerTest, SendMessageFailureReturnsFallback) {
  auto service = std::make_unique<MockMessageService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, sendMessage(HasSubstr("senderUuid"))).WillOnce(Return(std::nullopt));
  MessageHandler handler(std::move(service));

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"send_message","content":"hello"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("message_sent"));
}

TEST(MessageHandlerTest, ServiceExceptionsReturnServerErrors) {
  auto service = std::make_unique<MockMessageService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, sendMessage(_))
      .WillOnce(::testing::Throw(std::runtime_error("database")));
  MessageHandler handler(std::move(service));

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"send_message","content":"hello"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("Send failed: server error"));
}

TEST(MessageHandlerTest, UnknownOrEmptyTypeDoesNotCallService) {
  auto service = std::make_unique<MockMessageService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, sendMessage(_)).Times(0);
  EXPECT_CALL(*serviceMock, loadConversations(_)).Times(0);
  MessageHandler handler(std::move(service));

  handler.handle_type("u1", "{}", [](std::string) {});
  handler.handle_type("u1", R"({"type":"unknown"})", [](std::string) {});
}
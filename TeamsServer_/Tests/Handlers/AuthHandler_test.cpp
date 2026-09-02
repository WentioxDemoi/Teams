#include "Handlers/AuthHandler.h"
#include "../Mocks/AuthServiceMock.h"
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
}

TEST(AuthHandlerTest, Login_RoutesPayloadAndReturnsFormattedUser) {
  auto service = std::make_shared<MockAuthService>();
  User user;
  user.uuid = "u1";
  user.email = "me@example.com";
  EXPECT_CALL(*service, loginUser(testing::Field(&User::email, "me@example.com")))
      .WillOnce(Return(user));
  AuthHandler handler(service);

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type(R"({"type":"login","email":"me@example.com","password":"secret"})", callback);
  });
  EXPECT_THAT(response, HasSubstr(R"("type":"login_response")"));
}

TEST(AuthHandlerTest, RegisterFailureReturnsError) {
  auto service = std::make_shared<MockAuthService>();
  EXPECT_CALL(*service, registerUser(_)).WillOnce(Return(std::nullopt));
  AuthHandler handler(service);

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type(R"({"type":"register","email":"new@example.com","password":"secret"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("Registration failed"));
}

TEST(AuthHandlerTest, ValidateTokenRoutesToken) {
  auto service = std::make_shared<MockAuthService>();
  User user;
  user.token = "new-token";
  EXPECT_CALL(*service, validateToken("token-1")).WillOnce(Return(user));
  AuthHandler handler(service);

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type(R"({"type":"validate_token","token":"token-1"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("validate_token_response"));
}

TEST(AuthHandlerTest, LoginServiceExceptionReturnsServerError) {
  auto service = std::make_shared<MockAuthService>();
  EXPECT_CALL(*service, loginUser(_)).WillOnce(Return(std::optional<User>{}));
  AuthHandler handler(service);

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_login(R"({"email":"me@example.com","password":"secret"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("Login failed: invalid username"));
}

TEST(AuthHandlerTest, RegisterServiceExceptionReturnsServerError) {
  auto service = std::make_shared<MockAuthService>();
  EXPECT_CALL(*service, registerUser(_))
      .WillOnce(::testing::Throw(std::runtime_error("database")));
  AuthHandler handler(service);

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_register(R"({"email":"new@example.com","password":"secret"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("Register failed: server error"));
}

TEST(AuthHandlerTest, TokenServiceExceptionReturnsServerError) {
  auto service = std::make_shared<MockAuthService>();
  EXPECT_CALL(*service, validateToken("token-1"))
      .WillOnce(::testing::Throw(std::runtime_error("database")));
  AuthHandler handler(service);

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type(R"({"type":"validate_token","token":"token-1"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("Token validation failed: server error"));
}

TEST(AuthHandlerTest, UnknownOrEmptyTypeDoesNotCallService) {
  auto service = std::make_shared<MockAuthService>();
  EXPECT_CALL(*service, loginUser(_)).Times(0);
  EXPECT_CALL(*service, registerUser(_)).Times(0);
  EXPECT_CALL(*service, validateToken(_)).Times(0);
  AuthHandler handler(service);

  handler.handle_type("{}", [](std::string) {});
  handler.handle_type(R"({"type":"unknown"})", [](std::string) {});
}
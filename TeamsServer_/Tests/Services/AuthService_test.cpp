#include "Core/Services/AuthService.h"
#include "Utils/Crypto.h"
#include "../Mocks/UserRepositoryMock.h"
#include <optional>

using ::testing::_;
using ::testing::Return;

// ---------------------------------------------------------------------------
// loginUser
// ---------------------------------------------------------------------------

TEST(AuthServiceTest, LoginSuccess) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  User stored("uuid", "First", "Last", "me@example.com",
              Crypto::hash_password("secret"), /*token*/"", /*expires*/{}, /*created*/{}, /*updated*/{});
  std::optional<User> optStored = stored;

  EXPECT_CALL(*mockRepo, find_by_email("me@example.com"))
      .WillOnce(Return(optStored));
  EXPECT_CALL(*mockRepo, update(_)).WillOnce(Return(true));

  AuthService svc(mockRepo);
  User attempt;
  attempt.email = "me@example.com";
  attempt.plain_password = "secret";

  auto res = svc.loginUser(attempt);
  ASSERT_TRUE(res.has_value());
  EXPECT_EQ(res->email, "me@example.com");
}

TEST(AuthServiceTest, LoginSuccessSanitizesUserAndRotatesToken) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  User stored("uuid", "First", "Last", "me@example.com",
              Crypto::hash_password("secret"), "oldtoken", /*expires*/{}, /*created*/{}, /*updated*/{});
  std::optional<User> optStored = stored;

  EXPECT_CALL(*mockRepo, find_by_email("me@example.com"))
      .WillOnce(Return(optStored));
  EXPECT_CALL(*mockRepo, update(_)).WillOnce(Return(true));

  AuthService svc(mockRepo);
  User attempt;
  attempt.email = "me@example.com";
  attempt.plain_password = "secret";

  auto res = svc.loginUser(attempt);
  ASSERT_TRUE(res.has_value());
  EXPECT_TRUE(res->password_hash.empty());
  EXPECT_TRUE(res->plain_password.empty());
  EXPECT_NE(res->token, "oldtoken");
}

TEST(AuthServiceTest, LoginFailureWrongPassword) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  User stored("uuid", "First", "Last", "me@example.com",
              Crypto::hash_password("secret"), "", /*expires*/{}, /*created*/{}, /*updated*/{});
  std::optional<User> optStored = stored;

  EXPECT_CALL(*mockRepo, find_by_email("me@example.com"))
      .WillOnce(Return(optStored));

  AuthService svc(mockRepo);
  User attempt;
  attempt.email = "me@example.com";
  attempt.plain_password = "wrong";

  auto res = svc.loginUser(attempt);
  EXPECT_FALSE(res.has_value());
}

TEST(AuthServiceTest, LoginFailureWrongEmail) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  EXPECT_CALL(*mockRepo, find_by_email("me@example.com"))
      .WillOnce(Return(std::nullopt));

  AuthService svc(mockRepo);
  User attempt;
  attempt.email = "me@example.com";
  attempt.plain_password = "wrong";

  auto res = svc.loginUser(attempt);
  EXPECT_FALSE(res.has_value());
}

TEST(AuthServiceTest, LoginSuccessUpdateFailsStillReturnsUser) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  User stored("uuid", "First", "Last", "me@example.com",
              Crypto::hash_password("secret"), "", /*expires*/{}, /*created*/{}, /*updated*/{});
  std::optional<User> optStored = stored;

  EXPECT_CALL(*mockRepo, find_by_email("me@example.com"))
      .WillOnce(Return(optStored));
  EXPECT_CALL(*mockRepo, update(_)).WillOnce(Return(false));

  AuthService svc(mockRepo);
  User attempt;
  attempt.email = "me@example.com";
  attempt.plain_password = "secret";

  auto res = svc.loginUser(attempt);
  ASSERT_TRUE(res.has_value());
  EXPECT_EQ(res->email, "me@example.com");
}

// ---------------------------------------------------------------------------
// registerUser
// ---------------------------------------------------------------------------

TEST(AuthServiceTest, RegisterSuccess) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  EXPECT_CALL(*mockRepo, find_by_email("new@example.com"))
      .WillOnce(Return(std::optional<User>{}));

  EXPECT_CALL(*mockRepo, create(_)).WillOnce(Return(true));

  AuthService svc(mockRepo);
  User attempt;
  attempt.email = "new@example.com";
  attempt.plain_password = "mypassword";

  auto res = svc.registerUser(attempt);
  ASSERT_TRUE(res.has_value());
  EXPECT_EQ(res->email, "new@example.com");
}

TEST(AuthServiceTest, RegisterFailureEmailAlreadyExists) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  User stored("uuid", "First", "Last", "me@example.com",
              Crypto::hash_password("secret"), "", /*expires*/{}, /*created*/{}, /*updated*/{});
  std::optional<User> optStored = stored;

  EXPECT_CALL(*mockRepo, find_by_email("me@example.com"))
      .WillOnce(Return(optStored));
  EXPECT_CALL(*mockRepo, create(_)).Times(0);

  AuthService svc(mockRepo);
  User attempt;
  attempt.email = "me@example.com";
  attempt.plain_password = "whatever";

  auto res = svc.registerUser(attempt);
  EXPECT_FALSE(res.has_value());
}

TEST(AuthServiceTest, RegisterFailureCreateFails) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  EXPECT_CALL(*mockRepo, find_by_email("new@example.com"))
      .WillOnce(Return(std::optional<User>{}));
  EXPECT_CALL(*mockRepo, create(_)).WillOnce(Return(false));

  AuthService svc(mockRepo);
  User attempt;
  attempt.email = "new@example.com";
  attempt.plain_password = "mypassword";

  auto res = svc.registerUser(attempt);
  EXPECT_FALSE(res.has_value());
}

// ---------------------------------------------------------------------------
// validateToken
// ---------------------------------------------------------------------------

TEST(AuthServiceTest, ValidateTokenSuccess) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  User stored("uuid", "First", "Last", "me@example.com",
              Crypto::hash_password("secret"), "sometoken", /*expires*/{}, /*created*/{}, /*updated*/{});
  std::optional<User> optStored = stored;

  EXPECT_CALL(*mockRepo, find_by_token("sometoken"))
      .WillOnce(Return(optStored));

  AuthService svc(mockRepo);
  auto res = svc.validateToken("sometoken");
  ASSERT_TRUE(res.has_value());
  EXPECT_EQ(res->token, "sometoken");
}

TEST(AuthServiceTest, ValidateTokenFailure) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  User stored("uuid", "First", "Last", "me@example.com",
              Crypto::hash_password("secret"), "sometoken", /*expires*/{}, /*created*/{}, /*updated*/{});
  std::optional<User> optStored = stored;

  EXPECT_CALL(*mockRepo, find_by_token("wrongtoken"))
      .WillOnce(Return(std::nullopt));

  AuthService svc(mockRepo);
  auto res = svc.validateToken("wrongtoken");
  ASSERT_FALSE(res.has_value());
}

TEST(AuthServiceTest, ValidateTokenDoesNotSanitizePasswordHash) {
  auto mockRepo = std::make_shared<MockUserRepository>();

  User stored("uuid", "First", "Last", "me@example.com",
              Crypto::hash_password("secret"), "sometoken", /*expires*/{}, /*created*/{}, /*updated*/{});
  std::optional<User> optStored = stored;

  EXPECT_CALL(*mockRepo, find_by_token("sometoken"))
      .WillOnce(Return(optStored));

  AuthService svc(mockRepo);
  auto res = svc.validateToken("sometoken");

  ASSERT_TRUE(res.has_value());
  EXPECT_TRUE(res->password_hash.empty());
}

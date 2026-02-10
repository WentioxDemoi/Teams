
#include "../../src/Core/Models/User.h"
#include "../../src/Core/Services/AuthService.h"
#include "../../src/Utils/Crypto.h"
#include "../Mocks/MockUserRepository.h"
#include "../includes.h"

class AuthServiceTest : public ::testing::Test {
protected:
  std::unique_ptr<MockUserRepository> mockRepo;
  AuthService *authService;
  MockUserRepository *rawMock;

  void SetUp() override {
    mockRepo = std::make_unique<MockUserRepository>();
    rawMock = mockRepo.get();
    authService = new AuthService(std::move(mockRepo));
  }

  void TearDown() override { delete authService; }
};

TEST_F(AuthServiceTest, LoginUserSuccess) {
  auto user = make_test_user();
  EXPECT_CALL(*rawMock, find_by_email("alice@test.com")).WillOnce(Return(user));

  auto result = authService->loginUser("alice@test.com", "hashedpass");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->username, "Alice");
  EXPECT_TRUE(result->password_hash.empty());
}

TEST_F(AuthServiceTest, LoginUserWrongPassword) {
  auto user = make_test_user();
  EXPECT_CALL(*rawMock, find_by_email("alice@test.com")).WillOnce(Return(user));

  auto result = authService->loginUser("alice@test.com", "wrongpass");
  EXPECT_FALSE(result.has_value());
}

TEST_F(AuthServiceTest, LoginUserNotFound) {
  EXPECT_CALL(*rawMock, find_by_email("unknown@test.com"))
      .WillOnce(Return(std::nullopt));

  auto result = authService->loginUser("unknown@test.com", "pass123");
  EXPECT_FALSE(result.has_value());
}

TEST_F(AuthServiceTest, RegisterUserSuccess) {
  EXPECT_CALL(*rawMock, find_by_email("newuser@test.com"))
      .WillOnce(Return(std::nullopt));

  EXPECT_CALL(*rawMock, create(_)).WillOnce(Return(true));

  auto result =
      authService->registerUser("NewUser", "newuser@test.com", "pass123");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->username, "NewUser");
  EXPECT_TRUE(result->password_hash.empty());
}

TEST_F(AuthServiceTest, RegisterUserAlreadyExists) {
  auto existing = make_test_user();
  EXPECT_CALL(*rawMock, find_by_email("alice@test.com"))
      .WillOnce(Return(existing));

  auto result = authService->registerUser("Alice", "alice@test.com", "pass123");
  EXPECT_FALSE(result.has_value());
}

TEST_F(AuthServiceTest, ValidateTokenSuccess) {
  auto user = make_test_user();
  EXPECT_CALL(*rawMock, find_by_token("token123")).WillOnce(Return(user));

  auto result = authService->validateToken("token123");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->uuid, "uuid123");
}

TEST_F(AuthServiceTest, ValidateTokenFail) {
  EXPECT_CALL(*rawMock, find_by_token("badtoken"))
      .WillOnce(Return(std::nullopt));

  auto result = authService->validateToken("badtoken");
  EXPECT_FALSE(result.has_value());
}
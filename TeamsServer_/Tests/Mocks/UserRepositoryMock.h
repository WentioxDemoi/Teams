#include "Core/Repositories/UserRepository.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockUserRepository : public IUserRepository {
public:
  MOCK_METHOD(std::optional<User>, find_by_uuid, (const std::string &uuid),
              (override));

  MOCK_METHOD(std::optional<User>, find_by_email, (const std::string &email),
              (override));

  MOCK_METHOD(std::optional<User>, find_by_token, (const std::string &token),
              (override));

  MOCK_METHOD(std::vector<User>, search_by_name,
              (const std::string &callerUuid, const std::string &name),
              (override));

  MOCK_METHOD(bool, create, (const User &user), (override));
  MOCK_METHOD(bool, update, (const User &user), (override));
  MOCK_METHOD(bool, delete_user, (const std::string &uuid), (override));
};
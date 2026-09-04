#ifndef AUTHSERVICEMOCK_H
#define AUTHSERVICEMOCK_H

#include <gmock/gmock.h>

#include "Core/Services/IAuthService.h"

class MockAuthService : public IAuthService {
 public:
  MOCK_METHOD(std::optional<User>, loginUser, (const User& user), (override));
  MOCK_METHOD(std::optional<User>, registerUser, (const User& user), (override));
  MOCK_METHOD(std::optional<User>, validateToken, (const std::string& token), (override));
};

#endif
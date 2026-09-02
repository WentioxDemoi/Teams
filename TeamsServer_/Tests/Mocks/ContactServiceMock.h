#ifndef CONTACTSERVICEMOCK_H
#define CONTACTSERVICEMOCK_H

#include "Core/Services/IContactService.h"
#include <gmock/gmock.h>

class MockContactService : public IContactService {
 public:
  MOCK_METHOD(std::optional<std::string>, addContact, (const std::string &payload), (override));
  MOCK_METHOD(std::optional<std::string>, removeContact, (const std::string &payload), (override));
  MOCK_METHOD(std::optional<std::string>, loadContacts, (const std::string &payload), (override));
  MOCK_METHOD(std::optional<std::string>, searchUsers, (const std::string &userUuid, const std::string &payload), (override));
  MOCK_METHOD(std::optional<std::string>, resolveUserByUuid, (const std::string &uuid), (override));
  MOCK_METHOD(std::optional<std::string>, lastReadAt, (const std::string &uuid, const std::string &timestamp), (override));
  MOCK_METHOD(std::optional<std::string>, updateStatus, (const std::string &userUuid, const std::string &payload), (override));
};

#endif
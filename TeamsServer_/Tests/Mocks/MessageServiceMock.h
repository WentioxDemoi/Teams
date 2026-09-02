#ifndef MESSAGESERVICEMOCK_H
#define MESSAGESERVICEMOCK_H

#include "Core/Services/IMessageService.h"
#include <gmock/gmock.h>

class MockMessageService : public IMessageService {
 public:
  MOCK_METHOD(std::optional<std::string>, sendMessage, (const std::string &payload), (override));
  MOCK_METHOD(std::optional<std::string>, loadConversations, (const std::string &userUuid), (override));
};

#endif
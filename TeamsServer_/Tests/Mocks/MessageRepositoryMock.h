#include <gmock/gmock.h>

#include "Repositories/IMessageRepository.h"

class MockMessageRepository : public IMessageRepository {
 public:
  MOCK_METHOD(bool, save, (const Message& message), (override));
  MOCK_METHOD(bool, remove, (const std::string& uuid), (override));
  MOCK_METHOD(std::optional<std::vector<Message>>, findConversationsByUserUuid,
              (const std::string& userUuid), (override));
};
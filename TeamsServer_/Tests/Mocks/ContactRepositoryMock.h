#include <gmock/gmock.h>

#include "Repositories/IContactRepository.h"

class MockContactRepository : public IContactRepository {
 public:
  MOCK_METHOD(bool, create, (const Contact& contact), (override));
  MOCK_METHOD(bool, remove, (const std::string& userUuid, const std::string& contactUuid),
              (override));
  MOCK_METHOD(std::vector<User>, find_contact_owners, (const std::string& userUuid), (override));
  MOCK_METHOD(std::vector<User>, find_contacts, (const std::string& userUuid), (override));
  MOCK_METHOD(bool, update_last_read_at,
              (const std::string& userUuid, const std::string& contactUuid,
               const std::string& lastReadAt),
              (override));
};
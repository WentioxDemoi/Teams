#include "Core/Services/ContactService.h"

#include <gtest/gtest.h>

#include "../Mocks/ContactRepositoryMock.h"
#include "../Mocks/ContactSessionRegistryMock.h"
#include "../Mocks/UserRepositoryMock.h"

using ::testing::_;
using ::testing::Return;

namespace {

std::unique_ptr<ContactService> makeService(MockContactRepository*& rawContactRepo,
                                            std::shared_ptr<MockUserRepository>& userRepo,
                                            std::shared_ptr<MockContactSessionRegistry>& registry) {
  auto contactRepo = std::make_unique<MockContactRepository>();
  rawContactRepo = contactRepo.get();
  userRepo = std::make_shared<MockUserRepository>();
  registry = std::make_shared<MockContactSessionRegistry>();
  return std::make_unique<ContactService>(std::move(contactRepo), userRepo, registry);
}

}  // namespace

// ---------------------------------------------------------------------------
// addContact
// ---------------------------------------------------------------------------

TEST(ContactServiceTest, AddContact_CreateFails_ReturnsNullopt) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  EXPECT_CALL(*contactRepo, create(_)).WillOnce(Return(false));

  auto res = svc->addContact(R"({"userUuid":"u1","contactUuid":"u2"})");
  EXPECT_FALSE(res.has_value());
}

TEST(ContactServiceTest, AddContact_CreateSucceeds_ReturnsResponse) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  EXPECT_CALL(*contactRepo, create(_)).WillOnce(Return(true));

  auto res = svc->addContact(R"({"userUuid":"u1","contactUuid":"u2"})");
  ASSERT_TRUE(res.has_value());
}

// ---------------------------------------------------------------------------
// resolveUserByUuid
// ---------------------------------------------------------------------------

TEST(ContactServiceTest, ResolveUserByUuid_NotFound_ReturnsNullopt) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  EXPECT_CALL(*userRepo, find_by_uuid("u2")).WillOnce(Return(std::nullopt));

  auto res = svc->resolveUserByUuid(R"({"contactUuid":"u2"})");
  EXPECT_FALSE(res.has_value());
}

TEST(ContactServiceTest, ResolveUserByUuid_Found_SetsOnlineStatus) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  User u("u2", "First", "Last", "u2@example.com", "", "", {}, {}, {});
  EXPECT_CALL(*userRepo, find_by_uuid("u2")).WillOnce(Return(u));
  EXPECT_CALL(*registry, hasContactSession("u2")).WillOnce(Return(true));

  auto res = svc->resolveUserByUuid(R"({"contactUuid":"u2"})");
  ASSERT_TRUE(res.has_value());
}

// ---------------------------------------------------------------------------
// removeContact — implémentation actuellement désactivée (code commenté)
// ---------------------------------------------------------------------------

TEST(ContactServiceTest, RemoveContact_AlwaysReturnsNullopt_CurrentlyDisabled) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  EXPECT_CALL(*contactRepo, remove(_, _)).Times(0);

  auto res = svc->removeContact(R"({"userUuid":"u1","contactUuid":"u2"})");
  EXPECT_FALSE(res.has_value());
}

// ---------------------------------------------------------------------------
// loadContacts
// ---------------------------------------------------------------------------

TEST(ContactServiceTest, LoadContacts_EmptyUserUuid_ReturnsNulloptAndSkipsRepo) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  EXPECT_CALL(*contactRepo, find_contacts(_)).Times(0);

  auto res = svc->loadContacts(R"({"userUuid":""})");
  EXPECT_FALSE(res.has_value());
}

TEST(ContactServiceTest, LoadContacts_Success_SetsStatusPerContact) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  User c1("c1", "A", "A", "a@example.com", "", "", {}, {}, {});
  User c2("c2", "B", "B", "b@example.com", "", "", {}, {}, {});

  EXPECT_CALL(*contactRepo, find_contacts("u1")).WillOnce(Return(std::vector<User>{c1, c2}));
  EXPECT_CALL(*registry, hasContactSession("c1")).WillOnce(Return(true));
  EXPECT_CALL(*registry, hasContactSession("c2")).WillOnce(Return(false));

  auto res = svc->loadContacts(R"({"userUuid":"u1"})");
  ASSERT_TRUE(res.has_value());
}

// ---------------------------------------------------------------------------
// searchUsers
// ---------------------------------------------------------------------------

TEST(ContactServiceTest, SearchUsers_EmptyResult_ReturnsNullopt) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  EXPECT_CALL(*userRepo, search_by_name("u1", "john")).WillOnce(Return(std::vector<User>{}));
  EXPECT_CALL(*registry, hasContactSession(_)).Times(0);

  auto res = svc->searchUsers("u1", R"({"query":"john"})");
  EXPECT_FALSE(res.has_value());
}

TEST(ContactServiceTest, SearchUsers_Success_SetsStatus) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  User u("c1", "John", "Doe", "john@example.com", "", "", {}, {}, {});
  EXPECT_CALL(*userRepo, search_by_name("u1", "john")).WillOnce(Return(std::vector<User>{u}));
  EXPECT_CALL(*registry, hasContactSession("c1")).WillOnce(Return(true));

  auto res = svc->searchUsers("u1", R"({"query":"john"})");
  ASSERT_TRUE(res.has_value());
}

// ---------------------------------------------------------------------------
// lastReadAt
// ---------------------------------------------------------------------------

TEST(ContactServiceTest, LastReadAt_EmptyContactUuid_ReturnsNullopt) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  EXPECT_CALL(*contactRepo, update_last_read_at(_, _, _)).Times(0);

  auto res = svc->lastReadAt("u1", R"({"contactUuid":"","lastReadAt":"2024-01-01"})");
  EXPECT_FALSE(res.has_value());
}

TEST(ContactServiceTest, LastReadAt_UpdateFails_ReturnsNullopt) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  EXPECT_CALL(*contactRepo, update_last_read_at("u1", "c1", "2024-01-01")).WillOnce(Return(false));

  auto res = svc->lastReadAt("u1", R"({"contactUuid":"c1","lastReadAt":"2024-01-01"})");
  EXPECT_FALSE(res.has_value());
}

TEST(ContactServiceTest, LastReadAt_Success_ReturnsResponse) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  EXPECT_CALL(*contactRepo, update_last_read_at("u1", "c1", "2024-01-01")).WillOnce(Return(true));

  auto res = svc->lastReadAt("u1", R"({"contactUuid":"c1","lastReadAt":"2024-01-01"})");
  ASSERT_TRUE(res.has_value());
  EXPECT_NE(res->find("last_read_at_updated"), std::string::npos);
}

// ---------------------------------------------------------------------------
// updateStatus
// ---------------------------------------------------------------------------

TEST(ContactServiceTest, UpdateStatus_NotifiesOnlyOnlineRecipientsWithoutDuplicates) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  User contact("c1", "A", "A", "a@example.com", "", "", {}, {}, {});
  User owner("c1", "A", "A", "a@example.com", "", "", {}, {}, {});  // même uuid → doublon
  User owner2("c2", "B", "B", "b@example.com", "", "", {}, {}, {});

  EXPECT_CALL(*contactRepo, find_contacts("u1")).WillOnce(Return(std::vector<User>{contact}));
  EXPECT_CALL(*contactRepo, find_contact_owners("u1"))
      .WillOnce(Return(std::vector<User>{owner, owner2}));

  EXPECT_CALL(*registry, hasContactSession("c1")).WillOnce(Return(true));
  EXPECT_CALL(*registry, hasContactSession("c2")).WillOnce(Return(false));

  EXPECT_CALL(*registry, sendMessage("c1", _)).Times(1);
  EXPECT_CALL(*registry, sendMessage("c2", _)).Times(0);

  auto res = svc->updateStatus("u1", R"({"status":"Online"})");
  EXPECT_FALSE(res.has_value());
}

TEST(ContactServiceTest, UpdateStatus_NoRecipients_SendsNothing) {
  MockContactRepository* contactRepo;
  std::shared_ptr<MockUserRepository> userRepo;
  std::shared_ptr<MockContactSessionRegistry> registry;
  auto svc = makeService(contactRepo, userRepo, registry);

  EXPECT_CALL(*contactRepo, find_contacts("u1")).WillOnce(Return(std::vector<User>{}));
  EXPECT_CALL(*contactRepo, find_contact_owners("u1")).WillOnce(Return(std::vector<User>{}));
  EXPECT_CALL(*registry, sendMessage(_, _)).Times(0);

  auto res = svc->updateStatus("u1", R"({"status":"Offline"})");
  EXPECT_FALSE(res.has_value());
}
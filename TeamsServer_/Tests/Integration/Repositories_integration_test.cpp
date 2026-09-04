#include <gtest/gtest.h>

#include <chrono>

#include "Core/Repositories/ContactRepository.h"
#include "Core/Repositories/MessageRepository.h"
#include "Core/Repositories/UserRepository.h"
#include "Infrastructure/DatabaseManager.h"
#include "Utils/Config.h"

namespace {

class PostgreSQLIntegrationTest : public ::testing::Test {
 protected:
  void SetUp() override {
    if (std::getenv("DB_URL") == nullptr || std::getenv("DB_NAME") == nullptr) {
      GTEST_SKIP() << "DB_URL et DB_NAME sont requis pour les tests d'integration";
    }

    try {
      databaseManager_ = &DatabaseManager::instance();
      cleanDatabase();
    } catch (const std::exception& error) {
      GTEST_SKIP() << "PostgreSQL indisponible: " << error.what();
    }
  }

  void TearDown() override {
    if (databaseManager_ != nullptr) {
      try {
        cleanDatabase();
      } catch (...) {
      }
    }
  }

  void cleanDatabase() {
    auto connection = databaseManager_->acquire_connection();
    pqxx::work transaction(*connection);
    const auto& config = Config::instance();
    transaction.exec("DELETE FROM " + config.table_contacts());
    transaction.exec("DELETE FROM " + config.table_messages());
    transaction.exec("DELETE FROM " + config.table_users());
    transaction.commit();
    databaseManager_->release_connection(connection);
  }

  User makeUser(const std::string& uuid, const std::string& email,
                const std::string& firstName) const {
    User user;
    user.uuid = uuid;
    user.email = email;
    user.firstName = firstName;
    user.lastName = "Integration";
    user.password_hash = "hash-" + uuid;
    user.token = "token-" + uuid;
    user.token_expires_at = std::chrono::system_clock::now() + std::chrono::hours(1);
    user.created_at = std::chrono::system_clock::now();
    return user;
  }

  DatabaseManager* databaseManager_ = nullptr;
};

}  // namespace

TEST_F(PostgreSQLIntegrationTest, UserRepositoryCreatesReadsUpdatesSearchesAndDeletes) {
  UserRepository repository;
  auto user = makeUser("user-1", "alice@example.com", "Alice");

  ASSERT_TRUE(repository.create(user));

  const auto byUuid = repository.find_by_uuid(user.uuid);
  ASSERT_TRUE(byUuid.has_value());
  EXPECT_EQ(byUuid->email, user.email);
  EXPECT_EQ(byUuid->firstName, "Alice");

  const auto byEmail = repository.find_by_email(user.email);
  ASSERT_TRUE(byEmail.has_value());
  EXPECT_EQ(byEmail->uuid, user.uuid);

  const auto byToken = repository.find_by_token(user.token);
  ASSERT_TRUE(byToken.has_value());
  EXPECT_EQ(byToken->uuid, user.uuid);

  EXPECT_FALSE(repository.find_by_uuid("missing").has_value());
  EXPECT_FALSE(repository.find_by_token("missing").has_value());

  auto updated = user;
  updated.firstName = "Alicia";
  updated.lastName = "Updated";
  updated.email = "alicia@example.com";
  updated.token = "token-user-1-updated";
  ASSERT_TRUE(repository.update(updated));

  const auto afterUpdate = repository.find_by_uuid(user.uuid);
  ASSERT_TRUE(afterUpdate.has_value());
  EXPECT_EQ(afterUpdate->firstName, "Alicia");
  EXPECT_EQ(afterUpdate->email, updated.email);
  EXPECT_EQ(afterUpdate->token, updated.token);

  auto searchUser = makeUser("user-2", "alice.other@example.com", "Alison");
  ASSERT_TRUE(repository.create(searchUser));
  const auto searchResults = repository.search_by_name(user.uuid, "ali");
  ASSERT_EQ(searchResults.size(), 1);
  EXPECT_EQ(searchResults.front().uuid, searchUser.uuid);

  EXPECT_TRUE(repository.delete_user(user.uuid));
  EXPECT_FALSE(repository.delete_user(user.uuid));
}

TEST_F(PostgreSQLIntegrationTest, UserRepositoryRejectsExpiredTokens) {
  UserRepository repository;
  auto user = makeUser("expired-user", "expired@example.com", "Expired");
  user.token_expires_at = std::chrono::system_clock::now() - std::chrono::hours(1);

  ASSERT_TRUE(repository.create(user));
  EXPECT_FALSE(repository.find_by_token(user.token).has_value());
}

TEST_F(PostgreSQLIntegrationTest, ContactRepositoryCreatesFindsOwnersAndUpdatesReadTimestamp) {
  UserRepository users;
  ContactRepository contacts;
  auto owner = makeUser("contact-owner", "owner@example.com", "Owner");
  auto contact = makeUser("contact-target", "target@example.com", "Target");
  ASSERT_TRUE(users.create(owner));
  ASSERT_TRUE(users.create(contact));

  Contact relation{owner.uuid, contact.uuid, {}};
  ASSERT_TRUE(contacts.create(relation));

  const auto ownedContacts = contacts.find_contacts(owner.uuid);
  ASSERT_EQ(ownedContacts.size(), 1);
  EXPECT_EQ(ownedContacts.front().uuid, contact.uuid);

  const auto owners = contacts.find_contact_owners(contact.uuid);
  ASSERT_EQ(owners.size(), 1);
  EXPECT_EQ(owners.front().uuid, owner.uuid);

  ASSERT_TRUE(contacts.update_last_read_at(owner.uuid, contact.uuid, "2026-09-02 12:00:00+00"));
  const auto updatedContacts = contacts.find_contacts(owner.uuid);
  ASSERT_EQ(updatedContacts.size(), 1);
  EXPECT_NE(updatedContacts.front().lastReadAt, "");
}

TEST_F(PostgreSQLIntegrationTest, MessageRepositorySavesFindsAndRemovesMessages) {
  MessageRepository repository;
  Message message;
  message.id = "message-1";
  message.sender_id = "sender-1";
  message.receiver_id = "receiver-1";
  message.chatType = "message";
  message.content = "Integration message";
  message.timestamp = "2026-09-02 12:00:00+00";

  ASSERT_TRUE(repository.save(message));

  const auto conversations = repository.findConversationsByUserUuid(message.sender_id);
  ASSERT_TRUE(conversations.has_value());
  ASSERT_EQ(conversations->size(), 1);
  EXPECT_EQ(conversations->front().id, message.id);
  EXPECT_EQ(conversations->front().content, message.content);

  EXPECT_TRUE(repository.remove(message.id));
  EXPECT_FALSE(repository.remove(message.id));
  const auto emptyConversations = repository.findConversationsByUserUuid(message.sender_id);
  ASSERT_TRUE(emptyConversations.has_value());
  EXPECT_TRUE(emptyConversations->empty());
}
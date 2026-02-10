// tests/UserRepository_test.cpp
#include "../../src/Core/Models/User.h"
#include "../../src/Core/Repositories/UserRepository.h"
#include "../../src/Utils/Config.h"
#include "../includes.h"

class UserRepositoryIntegration : public ::testing::Test {
protected:
  UserRepository repo;

  // Génère un utilisateur de test avec des UUID aléatoires pour éviter les
  // collisions
  User make_test_user() {
    auto now = std::chrono::system_clock::now();
    User u;
    u.uuid = "test-uuid-" + std::to_string(rand());
    u.username = "TestUser";
    u.email = "testuser" + std::to_string(rand()) + "@example.com";
    u.password_hash = "hashedpassword";
    u.token = "token123";
    u.token_expires_at = now + std::chrono::hours(1);
    u.status = "active";
    u.last_seen = now;
    u.created_at = now;
    return u;
  }

  void SetUp() override {
    // Optionnel : nettoyer certains users connus si nécessaire
  }

  void TearDown() override {
    // Supprime tous les utilisateurs créés dans ce test pour ne pas polluer la
    // DB
    for (const auto &uuid : created_uuids) {
      repo.delete_user(uuid);
    }
    created_uuids.clear();
  }

  std::vector<std::string> created_uuids;
};

TEST_F(UserRepositoryIntegration, CreateAndFindByUUID) {
  auto user = make_test_user();
  ASSERT_TRUE(repo.create(user));
  created_uuids.push_back(user.uuid);

  auto found = repo.find_by_uuid(user.uuid);
  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(found->username, user.username);
  EXPECT_EQ(found->email, user.email);
}

TEST_F(UserRepositoryIntegration, FindByEmail) {
  auto user = make_test_user();
  ASSERT_TRUE(repo.create(user));
  created_uuids.push_back(user.uuid);

  auto found = repo.find_by_email(user.email);
  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(found->uuid, user.uuid);
}

TEST_F(UserRepositoryIntegration, FindByToken) {
  auto user = make_test_user();
  ASSERT_TRUE(repo.create(user));
  created_uuids.push_back(user.uuid);

  auto found = repo.find_by_token(user.token);
  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(found->uuid, user.uuid);
}

TEST_F(UserRepositoryIntegration, UpdateUser) {
  auto user = make_test_user();
  ASSERT_TRUE(repo.create(user));
  created_uuids.push_back(user.uuid);

  user.username = "UpdatedName";
  user.status = "inactive";
  ASSERT_TRUE(repo.update(user));

  auto found = repo.find_by_uuid(user.uuid);
  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(found->username, "UpdatedName");
  EXPECT_EQ(found->status, "inactive");
}

TEST_F(UserRepositoryIntegration, DeleteUser) {
  auto user = make_test_user();
  ASSERT_TRUE(repo.create(user));

  ASSERT_TRUE(repo.delete_user(user.uuid));

  auto found = repo.find_by_uuid(user.uuid);
  EXPECT_FALSE(found.has_value());
}
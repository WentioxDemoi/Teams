#include "../../src/Core/Repositories/UserRepository.h"
#include "../includes.h"

/**
 * @class MockUserRepository
 * @brief Répertoire utilisateur simulé pour les tests unitaires.
 *
 * Fournit des versions mockées des méthodes de UserRepository pour permettre
 * le test de services ou gestionnaires dépendants sans accéder à une vraie base
 * de données.
 */
class MockUserRepository : public UserRepository {
public:
  MOCK_METHOD(std::optional<User>, find_by_email, (const std::string &),
              (override));
  MOCK_METHOD(std::optional<User>, find_by_token, (const std::string &),
              (override));
  MOCK_METHOD(bool, create, (const User &), (override));
};

std::optional<User> make_test_user() {
  return User{"uuid123",
              "Alice",
              "alice@test.com",
              Crypto::hash_password("hashedpass"),
              "token123",
              std::chrono::system_clock::now() + std::chrono::hours(1),
              "Online",
              std::chrono::system_clock::now(),
              std::chrono::system_clock::now()};
}
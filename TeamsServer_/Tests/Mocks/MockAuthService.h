#include "../../src/Core/Services/AuthService.h"
#include "../includes.h"

/**
 * @class MockAuthService
 * @brief Service d'authentification simulé pour les tests unitaires.
 *
 * Fournit des versions mockées des méthodes d'AuthService pour permettre
 * le test des composants dépendants sans interagir avec la base de données
 * réelle.
 */
class MockAuthService : public AuthService {
public:
  MOCK_METHOD(std::optional<User>, loginUser,
              (const std::string &, const std::string &), (override));
  MOCK_METHOD(std::optional<User>, registerUser,
              (const std::string &, const std::string &, const std::string &),
              (override));
  MOCK_METHOD(std::optional<User>, validateToken, (const std::string &),
              (override));
};

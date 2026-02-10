#include "../../src/Utils/Crypto.h"
#include "../includes.h"

TEST(CryptoTest, HashAndVerifySuccess) {
  std::string password = "SuperSecret123";

  std::string hash = Crypto::hash_password(password);

  EXPECT_TRUE(Crypto::verify_password(password, hash));
}

TEST(CryptoTest, VerifyFailsWithWrongPassword) {
  std::string password = "SuperSecret123";
  std::string wrong = "WrongPassword";

  std::string hash = Crypto::hash_password(password);

  EXPECT_FALSE(Crypto::verify_password(wrong, hash));
}

TEST(CryptoTest, SamePasswordGeneratesDifferentHashes) {
  std::string password = "SuperSecret123";

  std::string h1 = Crypto::hash_password(password);
  std::string h2 = Crypto::hash_password(password);

  EXPECT_NE(h1, h2);
}

TEST(CryptoTest, HashHasArgon2Prefix) {
  std::string hash = Crypto::hash_password("password");

  EXPECT_TRUE(hash.rfind("$argon2id$", 0) == 0);
}

TEST(CryptoTest, TokenFormat) {
  std::string token = Crypto::generate_token();

  EXPECT_EQ(token.size(), 32u);
  EXPECT_TRUE(std::all_of(token.begin(), token.end(), [](char c) {
    return std::isxdigit(static_cast<unsigned char>(c));
  }));
}

TEST(CryptoTest, UUIDFormat) {
  std::string uuid = Crypto::generate_uuid();

  std::regex uuid_v4(
      "^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$");

  EXPECT_TRUE(std::regex_match(uuid, uuid_v4));
}
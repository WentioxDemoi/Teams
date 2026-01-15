#ifndef REPOSITORIESTOOLS_H
#define REPOSITORIESTOOLS_H

#include "../includes.h"

class RepositoriesTools {

public:
  static std::string hash_password(const std::string &password) {
    const uint32_t t_cost = 3;
    const uint32_t m_cost =
        1
        << 16; // 64 MB | A augmenter/réduire en fonction des ressources/besoins
    const uint32_t parallelism = 1;
    const size_t salt_len = 16;
    const size_t hash_len = 32;

    std::vector<uint8_t> salt(salt_len);
    std::random_device rd;
    for (auto &b : salt)
      b = static_cast<uint8_t>(rd());

    // Calcul taille exacte de la string encodée
    size_t encoded_len = argon2_encodedlen(t_cost, m_cost, parallelism,
                                           salt_len, hash_len, Argon2_id);

    std::vector<char> encoded(encoded_len);

    int rc = argon2id_hash_encoded(t_cost, m_cost, parallelism, password.data(),
                                   password.size(), salt.data(), salt.size(),
                                   hash_len, encoded.data(), encoded.size());

    if (rc != ARGON2_OK)
      throw std::runtime_error(argon2_error_message(rc));

    return std::string(encoded.data());
  }

  static bool verify_password(const std::string &password,
                              const std::string &hash) {
    return argon2id_verify(hash.c_str(), password.data(), password.size()) ==
           ARGON2_OK;
  }

  static std::string generate_token() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    for (int i = 0; i < 32; ++i)
      ss << std::hex << dis(gen);
    return ss.str();
  }

  RepositoriesTools() = delete;
  ~RepositoriesTools() = delete;
};

#endif
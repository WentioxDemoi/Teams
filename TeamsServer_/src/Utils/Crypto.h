#ifndef CRYPTO_H
#define CRYPTO_H

#include "../includes.h"

/**
 * @class Crypto
 * @brief Utilitaires statiques pour la sécurité : hachage, vérification et
 * génération.
 *
 * Fournit des méthodes pour sécuriser les mots de passe, générer des tokens et
 * créer des UUID.
 */
class Crypto {

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

static std::string generate_uuid_v4() {
    static std::mt19937 rng(std::random_device{}()); // initialisé une seule fois
    std::uniform_int_distribution<int> dist(0, 15);
    std::uniform_int_distribution<int> dist2(8, 11);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; i++)  ss << dist(rng);
    ss << "-";
    for (int i = 0; i < 4; i++)  ss << dist(rng);
    ss << "-4";
    for (int i = 0; i < 3; i++)  ss << dist(rng);
    ss << "-";
    ss << dist2(rng);
    for (int i = 0; i < 3; i++)  ss << dist(rng);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dist(rng);
    return ss.str();
}


static std::string generate_uuid_v7() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint64_t> dist(0, 0xFFF);
    std::uniform_int_distribution<uint64_t> dist64(0, 0x3FFFFFFFFFFFFFFF);

    // Timestamp en millisecondes (48 bits)
    auto now = std::chrono::system_clock::now();
    uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();

    uint64_t rand_a = dist(rng);    // 12 bits
    uint64_t rand_b = dist64(rng);  // 62 bits

    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    // time_high (32 bits)
    ss << std::setw(8) << ((ms >> 16) & 0xFFFFFFFF);
    ss << "-";
    // time_mid (16 bits)
    ss << std::setw(4) << (ms & 0xFFFF);
    ss << "-";
    // version (4 bits) + rand_a (12 bits)
    ss << std::setw(4) << (0x7000 | rand_a);
    ss << "-";
    // variant (2 bits) + rand_b partiel (14 bits)
    ss << std::setw(4) << (0x8000 | (rand_b >> 48));
    ss << "-";
    // rand_b restant (48 bits)
    ss << std::setw(12) << (rand_b & 0xFFFFFFFFFFFF);

    return ss.str();
}

  Crypto() = delete;
};


#endif
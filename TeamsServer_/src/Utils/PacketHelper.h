#ifndef PACKETHELPER_H
#define PACKETHELPER_H

#include "../includes.h"

/**
 * @class PacketHelper
 * @brief Outils utilitaires pour la manipulation des paquets réseau.
 *
 * Fournit des fonctions statiques permettant d'extraire, insérer
 * ou remplacer des valeurs dans des payloads JSON simplifiés.
 */
class PacketHelper {
public:
  static std::string extractValue(const std::string& payload,
                                  const std::string& key) {
    std::string searchKey = R"(")" + key + R"(":")";
    auto pos = payload.find(searchKey);
    if (pos == std::string::npos)
      return {};

    pos += searchKey.length();
    auto end = payload.find('"', pos);
    if (end == std::string::npos)
      return {};

    return payload.substr(pos, end - pos);
  }

  static void insertValue(std::string& payload,
                          const std::string& key,
                          const std::string& value) {
    std::string searchKey = R"(")" + key + R"(":")";

    auto pos = payload.find(searchKey);

    if (pos != std::string::npos) {
      // Remplacement d'une valeur existante
      auto valueStart = pos + searchKey.length();
      auto valueEnd = payload.find('"', valueStart);

      if (valueEnd != std::string::npos) {
        payload.replace(valueStart, valueEnd - valueStart, value);
      }
    } else {
      // Insertion d'une nouvelle paire clé/valeur
      auto insertPos = payload.rfind('}');

      if (insertPos != std::string::npos) {
        bool hasEntries = payload.find(':') != std::string::npos;

        std::string entry =
            (hasEntries ? "," : "") +
            std::string("\"") + key + "\":\"" + value + "\"";

        payload.insert(insertPos, entry);
      }
    }
  }

  PacketHelper() = delete;
};

#endif
#ifndef HANDLERTOOLS_H
#define HANDLERTOOLS_H

#include "../includes.h"

/**
 * @class HandlerTools
 * @brief Outils utilitaires pour le traitement des requêtes payload.
 *
 * Fournit des fonctions statiques pour extraire des valeurs spécifiques à
 * partir de chaînes JSON simplifiées.
 */
class HandlerTools {
public:
  static std::string extractValue(const std::string &payload,
                                  const std::string &key) {
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

  HandlerTools() = delete;
};

#endif
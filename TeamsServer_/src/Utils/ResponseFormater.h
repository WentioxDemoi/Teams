#ifndef RESPONSEFORMATER_H
#define RESPONSEFORMATER_H

#include "../Core/Models/User.h"
#include "../includes.h"

/**
 * @class ResponseFormater
 * @brief Utilitaires statiques pour formater les réponses JSON côté serveur.
 *
 * Fournit des fonctions pour construire des réponses structurées et échapper
 * correctement les chaînes afin de produire du JSON valide.
 */
class ResponseFormater {
public:
  static std::string format_user_response(const std::string &type,
                                          const User &user) {
    return "{"
           "\"type\":\"" +
           type +
           "\","
           "\"data\":{"
           "\"username\":\"" +
           json_escape(user.username) +
           "\","
           "\"email\":\"" +
           json_escape(user.email) +
           "\","
           "\"uuid\":\"" +
           json_escape(user.uuid) +
           "\","
           "\"token\":\"" +
           json_escape(user.token) +
           "\","
           "\"status\":\"" +
           json_escape(user.status) +
           "\""
           "}"
           "}";
  }

  static std::string json_escape(const std::string &str) {
    std::string result;
    result.reserve(str.size());

    for (char c : str) {
      switch (c) {
      case '"':
        result += "\\\"";
        break;
      case '\\':
        result += "\\\\";
        break;
      case '\b':
        result += "\\b";
        break;
      case '\f':
        result += "\\f";
        break;
      case '\n':
        result += "\\n";
        break;
      case '\r':
        result += "\\r";
        break;
      case '\t':
        result += "\\t";
        break;
      default:
        result += c;
        break;
      }
    }
    return result;
  }

  ResponseFormater() = delete;
};

#endif

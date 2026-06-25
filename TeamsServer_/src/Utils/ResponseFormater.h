#ifndef RESPONSEFORMATER_H
#define RESPONSEFORMATER_H

#include "../Core/Models/Contact.h"
#include "../Core/Models/Message.h"
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
  static std::string format_user_response(const std::string &type, const User &user) {
    return "{"
           "\"type\":\"" +
           type +
           "\","
           "\"data\":{"
           "\"firstName\":\"" +
           json_escape(user.firstName) +
           "\","
           "\"lastName\":\"" +
           json_escape(user.lastName) +
           "\","
           "\"email\":\"" +
           json_escape(user.email) +
           "\","
           "\"uuid\":\"" +
           json_escape(user.uuid) +
           "\","
           "\"token\":\"" +
           json_escape(user.token) + "\"" +
           "}"
           "}";
  }

  static std::string format_user_list_response(const std::string &type, const std::vector<User> &users) {
    std::string result = "{"
                         "\"type\":\"" +
                         type +
                         "\","
                         "\"data\":[";

    for (size_t i = 0; i < users.size(); ++i) {
      const User &user = users[i];
      result += "{"
                "\"firstName\":\"" +
                json_escape(user.firstName) +
                "\","
                "\"lastName\":\"" +
                json_escape(user.lastName) +
                "\","
                "\"email\":\"" +
                json_escape(user.email) +
                "\","
                "\"uuid\":\"" +
                json_escape(user.uuid) +
                "\","
                "\"lastReadAt\":\"" +
                json_escape(user.lastReadAt) + "\"" + "}";
      if (i + 1 < users.size())
        result += ",";
    }

    result += "]}";
    return result;
  }

  static std::string format_message_response(const std::string &type, const Message &message) {
    return "{\"type\":\"" + type + "\",\"data\":" + message_to_json_object(message) + "}";
  }

  static std::string format_conversations_response(const std::string &type, const std::vector<Message> &messages) {
    std::string result = "{\"type\":\"" + type + "\",\"data\":[";
    for (size_t i = 0; i < messages.size(); ++i) {
      result += message_to_json_object(messages[i]);
      if (i + 1 < messages.size())
        result += ",";
    }
    result += "]}";
    return result;
  }

  static std::string format_contact_response(const std::string &type, const Contact &contact) {
    return "{"
           "\"type\":\"" +
           type +
           "\","
           "\"data\":{"
           "\"userUuid\":\"" +
           json_escape(contact.user_id) +
           "\","
           "\"contactUuid\":\"" +
           json_escape(contact.contact_id) +
           "\""
           "}"
           "}";
  }

private:
  static std::string message_to_json_object(const Message &message) {
    return "{"
           "\"uuid\":\"" +
           json_escape(message.id) +
           "\","
           "\"senderUuid\":\"" +
           json_escape(message.sender_id) +
           "\","
           "\"receiverUuid\":\"" +
           json_escape(message.receiver_id) +
           "\","
           "\"chatType\":\"" +
           json_escape(message.chatType) +
           "\","
           "\"content\":\"" +
           json_escape(message.content) +
           "\","
           "\"timestamp\":\"" +
           json_escape(message.timestamp) +
           "\""
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
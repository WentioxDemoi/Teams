#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <cstdint>

enum class MessageType : uint8_t {
    CHAT,
    SYSTEM,
    PRESENCE,
    FILE,
};

struct Message {
    std::string id;           // UUID v7
    std::string sender_id;    // UUID v4
    std::string receiver_id;  // UUID v4 (vide pour SYSTEM/PRESENCE)
    MessageType type = MessageType::CHAT;
    std::string content;
    std::string timestamp;    // ISO 8601, ex: "2025-01-15T14:30:00.000Z"
    bool is_read = false;
};

inline std::string message_type_to_string(MessageType type) {
    switch (type) {
        case MessageType::CHAT:     return "chat";
        case MessageType::SYSTEM:   return "system";
        case MessageType::PRESENCE: return "presence";
        case MessageType::FILE:     return "file";
        default:                    return "chat";
    }
}

inline MessageType message_type_from_string(const std::string& str) {
    if (str == "system")   return MessageType::SYSTEM;
    if (str == "presence") return MessageType::PRESENCE;
    if (str == "file")     return MessageType::FILE;
    return MessageType::CHAT;
}

#endif
#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <cstdint>
#include "../../Utils/PacketHelper.h"
#include <sstream>

struct Message {
    std::string id;           // UUID v7
    std::string sender_id;    // UUID v4
    std::string receiver_id;  // UUID v4 (vide pour SYSTEM/PRESENCE)
    std::string chatType = "message";
    std::string content;
    std::string timestamp;    // ISO 8601, ex: "2025-01-15T14:30:00.000Z"
    bool is_read = false;
};

inline std::string message_to_json(const Message& msg) {
    std::ostringstream oss;
    oss << "{"
        << "\"id\":\"" << msg.id << "\","
        << "\"sender_id\":\"" << msg.sender_id << "\","
        << "\"receiver_id\":\"" << msg.receiver_id << "\","
        << "\"chatType\":\"" << msg.chatType << "\","
        << "\"content\":\"" << msg.content << "\","
        << "\"timestamp\":\"" << msg.timestamp << "\","
        << "\"is_read\":" << (msg.is_read ? "true" : "false")
        << "}";
    return oss.str();
}

inline Message message_from_json(const std::string& json) {
    Message msg;
    msg.id          = PacketHelper::extractValue(json, "id");
    msg.sender_id   = PacketHelper::extractValue(json, "sender_id");
    msg.receiver_id = PacketHelper::extractValue(json, "receiver_id");
    msg.chatType    = PacketHelper::extractValue(json, "chatType");
    msg.content     = PacketHelper::extractValue(json, "content");
    msg.timestamp   = PacketHelper::extractValue(json, "timestamp");
    msg.is_read     = PacketHelper::extractValue(json, "is_read") == "true";
    return msg;
}

#endif
#ifndef CONTACT_H
#define CONTACT_H

#include <string>
#include <cstdint>
#include "../../Utils/PacketHelper.h"
#include <sstream>

struct Contact {
    std::string user_id;      // UUID v4 - propriétaire de la liste de contacts
    std::string contact_id;   // UUID v4 - contact ajouté
    std::string created_at;   // ISO 8601, ex: "2025-01-15T14:30:00.000Z"
};

inline Contact contact_from_json(const std::string& json) {
    Contact contact;
    contact.user_id    = PacketHelper::extractValue(json, "userUuid");
    contact.contact_id = PacketHelper::extractValue(json, "contactUuid");
    contact.created_at = PacketHelper::extractValue(json, "createdAt");
    return contact;
}

#endif
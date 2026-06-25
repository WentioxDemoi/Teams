#include "ContactService.h"
#include "../../Utils/ResponseFormater.h"
#include "PacketHelper.h"
#include "Repositories/UserRepository.h"
#include <optional>

std::optional<std::string> ContactService::addContact(const std::string &payload) {

  Contact contact = contact_from_json(payload);

  if (!contactRepo_->create(contact)) {
    return std::nullopt;
  }

  std::string response = ResponseFormater::format_contact_response("contact_added", contact);

  return response;
}

std::optional<std::string> ContactService::resolveUserByUuid(const std::string &payload) {
  auto userUuid = PacketHelper::extractValue(payload, "contactUuid");
  std::optional<User> user = userRepo_->find_by_uuid(userUuid);

  if (!user.has_value()) {
    return std::nullopt;
  }

  std::string response = ResponseFormater::format_user_response("resolve_user_response", user.value());

  return response;
}

std::optional<std::string> ContactService::removeContact(const std::string &payload) {

  //   auto userUuid =
  //       PacketHelper::extractValue(payload, "userUuid");

  //   auto contactUuid =
  //       PacketHelper::extractValue(payload, "contactUuid");

  //   if (userUuid.empty() || contactUuid.empty()) {
  //     return std::nullopt;
  //   }

  //   if (!contactRepo_->remove(userUuid, contactUuid)) {
  //     return std::nullopt;
  //   }

  //   return R"({
  //     "type":"contact_removed",
  //     "success":true
  //   })";
}

std::optional<std::string> ContactService::loadContacts(const std::string &payload) {

  auto userUuid = PacketHelper::extractValue(payload, "userUuid");

  if (userUuid.empty()) {
    return std::nullopt;
  }

  auto contacts = contactRepo_->find_contacts(userUuid);

  for (User &contact : contacts) {
    contact.status = contactSessionRegistry_->hasContactSession(contact.uuid) ? "Online" : "Offline";
  }

  return ResponseFormater::format_user_list_response("contacts_loaded", contacts);
}

std::optional<std::string> ContactService::searchUsers(const std::string &userUuid, const std::string &payload) {
  const std::string query = PacketHelper::extractValue(payload, "query");

  auto result = userRepo_->search_by_name(userUuid, query);

  if (!result.empty())
    return ResponseFormater::format_user_list_response("search_users_response", result);
  else
    return std::nullopt;
}

std::optional<std::string> ContactService::lastReadAt(const std::string &userUuid, const std::string &payload) {

  auto contactUuid = PacketHelper::extractValue(payload, "contactUuid");
  auto lastReadAt = PacketHelper::extractValue(payload, "lastReadAt");

  if (contactUuid.empty()) {
    return std::nullopt;
  }

  if (!contactRepo_->update_last_read_at(userUuid, contactUuid, lastReadAt)) {
    return std::nullopt;
  }

  return R"({"type":"last_read_at_updated","success":true})";
}

std::optional<std::string> ContactService::updateStatus(
    const std::string &userUuid,
    const std::string &payload) {

    const std::string status =
        PacketHelper::extractValue(payload, "status");

    auto contacts = contactRepo_->find_contacts(userUuid);

    const std::string notification =
        ResponseFormater::format_contact_status_update_response(
            userUuid,
            status);

    for (User &contact : contacts) {
        if (contactSessionRegistry_->hasContactSession(contact.uuid)) {
            contactSessionRegistry_->sendMessage(
                contact.uuid,
                notification);
        }
    }

    return std::nullopt;
}
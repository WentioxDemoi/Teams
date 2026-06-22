#include "ContactService.h"
#include "PacketHelper.h"
#include "Repositories/UserRepository.h"
#include <optional>
#include "../../Utils/ResponseFormater.h"

std::optional<std::string>
ContactService::addContact(const std::string &payload) {

    Contact contact = contact_from_json(payload);

    if (!contactRepo_->create(contact)) {
      return std::nullopt;
    }

    std::string response =
        ResponseFormater::format_contact_response(
            "contact_added",
            contact);

    return response;
}

std::optional<std::string>
ContactService::removeContact(const std::string &payload) {

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

std::optional<std::string>
ContactService::loadContacts(const std::string &payload) {

    auto userUuid =
        PacketHelper::extractValue(payload, "userUuid");

    if (userUuid.empty()) {
      return std::nullopt;
    }

    auto contacts =
        contactRepo_->find_contacts(userUuid);

    return ResponseFormater::format_user_list_response(
        "contacts_loaded",
        contacts);
}

std::optional<std::vector<User>> ContactService::searchUsers(const std::string &callerUuid, const std::string &payload)
{
    const std::string query = PacketHelper::extractValue(payload, "query");

    auto result = userRepo_->search_by_name(callerUuid, query);
    
    if (!result.empty())
        return result;
    else
        return std::nullopt;
}
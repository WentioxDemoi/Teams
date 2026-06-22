#include "ContactHandler.h"
#include "../Utils/PacketHelper.h"
#include "../Utils/ResponseFormater.h"

#include <optional>

void ContactHandler::handle_add_contact(std::string uuid, std::string payload,
                                        ResponseCallback respond) {

  std::cout << "[ContactHandler] Adding contact with payload: " << payload
            << std::endl;

    asio::post(worker_pool_, [this, payload, respond]() {
      try {
        std::optional<std::string> response =
            contactService_->addContact(payload);

        if (!response.has_value()) {
          response =
              R"({"type":"contact_added","error":"Add failed: invalid
              data."})";
        }

        respond(response.value());

      } catch (const std::exception &e) {
        std::cerr << "[ContactHandler] Add error: " << e.what() << "\n";

        respond(R"({"type":"contact_added","error":"Add failed: server
        error"})");
      }
    });
}

void ContactHandler::handle_remove_contact(std::string uuid,
                                           std::string payload,
                                           ResponseCallback respond) {

  std::cout << "[ContactHandler] Removing contact with payload: " << payload
            << std::endl;

  //   asio::post(worker_pool_, [this, payload, respond]() {
  //     try {
  //       std::optional<std::string> response =
  //           contactService_->removeContact(payload);

  //       if (!response.has_value()) {
  //         response =
  //             R"({"type":"contact_removed","error":"Remove failed: invalid
  //             data."})";
  //       }

  //       respond(response.value());

  //     } catch (const std::exception &e) {
  //       std::cerr << "[ContactHandler] Remove error: " << e.what() << "\n";

  //       respond(
  //           R"({"type":"contact_removed","error":"Remove failed: server
  //           error"})");
  //     }
  //   });
}

void ContactHandler::handle_load_contacts(std::string uuid, std::string payload,
                                          ResponseCallback respond) {

  std::cout << "[ContactHandler] Loading contacts with payload: " << payload
            << std::endl;

    asio::post(worker_pool_, [this, payload, respond]() {
      try {
        std::optional<std::string> response =
            contactService_->loadContacts(payload);

        if (!response.has_value()) {
          response = R"({"type":"contacts_loaded","error":"Load failed."})";
        }

        respond(response.value());

      } catch (const std::exception &e) {
        std::cerr << "[ContactHandler] Load error: " << e.what() << "\n";

        respond(
            R"({"type":"contacts_loaded","error":"Load failed: server
            error"})");
      }
    });
}

void ContactHandler::handle_search_users(const std::string &uuid,
                                            std::string payload,
                                            ResponseCallback respond) {

  std::cout << "[ContactHandler] Searching contacts with payload: " << payload
            << std::endl;

  asio::post(worker_pool_, [this, uuid, payload, respond]() {
    try {
      auto response = contactService_->searchUsers(uuid, payload);
      std::string result;

      if (!response.has_value()) {
        result = R"({"type":"contacts_loaded","error":"Load failed."})";
      } else {
        result = ResponseFormater::format_user_list_response("search_users_response", response.value());
      }
      respond(result);
    } catch (const std::exception &e) {
      std::cerr << "[ContactHandler] Search error: " << e.what() << "\n";

      respond(
          R"({"type":"users_searched,"error":"Search failed: server error"})");
    }
  });
}

void ContactHandler::handle_type(std::string uuid, std::string payload,
                                 ResponseCallback respond) {

  std::string type = PacketHelper::extractValue(payload, "type");
  PacketHelper::insertValue(payload, "userUuid", uuid);
  std::cout << "type : " << type << std::endl;

  if (type.empty())
    return;

  if (type == "add_contact") {
    handle_add_contact(uuid, payload, respond);
  } else if (type == "remove_contact") {
    handle_remove_contact(uuid, payload, respond);
  } else if (type == "load_contacts") {
    handle_load_contacts(uuid, payload, respond);
  } else if (type == "search_users") {
    handle_search_users(uuid, payload, respond);
  } else {
    std::cerr << "[ContactHandler] Unknown contact type: " << type << "\n";
  }
}
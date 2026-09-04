#ifndef ICONTACTSERVICE_H
#define ICONTACTSERVICE_H

#include <optional>
#include <string>

/**
 * @class IContactService
 * @brief Contrat métier pour la gestion des contacts.
 */
class IContactService {
 public:
  virtual ~IContactService() = default;

  virtual std::optional<std::string> addContact(const std::string& payload) = 0;
  virtual std::optional<std::string> removeContact(const std::string& payload) = 0;
  virtual std::optional<std::string> loadContacts(const std::string& payload) = 0;
  virtual std::optional<std::string> searchUsers(const std::string& userUuid,
                                                 const std::string& payload) = 0;
  virtual std::optional<std::string> resolveUserByUuid(const std::string& uuid) = 0;
  virtual std::optional<std::string> lastReadAt(const std::string& uuid,
                                                const std::string& timestamp) = 0;
  virtual std::optional<std::string> updateStatus(const std::string& userUuid,
                                                  const std::string& payload) = 0;
};

#endif

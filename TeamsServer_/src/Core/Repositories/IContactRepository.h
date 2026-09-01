#ifndef ICONTACT_REPOSITORY_H
#define ICONTACT_REPOSITORY_H

#include "../Models/Contact.h"
#include "../Models/User.h"

#include <string>
#include <vector>

class IContactRepository {
public:
  virtual ~IContactRepository() = default;

  virtual bool create(const Contact &contact) = 0;

  virtual bool remove(const std::string &userUuid, const std::string &contactUuid) = 0;

  virtual std::vector<User> find_contact_owners(const std::string &userUuid) = 0;
  virtual std::vector<User> find_contacts(const std::string &userUuid) = 0;

  virtual bool update_last_read_at(const std::string &userUuid, const std::string &contactUuid,
                                    const std::string &lastReadAt) = 0;
};

#endif
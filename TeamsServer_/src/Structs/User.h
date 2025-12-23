#ifndef USER_H
#define USER_H

#include <string>

struct User {
  int id;
  std::string username;
  std::string token;
  std::string status;
};

#endif
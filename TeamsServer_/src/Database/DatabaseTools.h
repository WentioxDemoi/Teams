#ifndef DATABASETOOLS_H
#define DATABASETOOLS_H

#include "../includes.h"

class DatabaseTools {

public:
  // Conversion time_point -> string car pqxx ne gere pas ce type (pour
  // insertion dans la BDD)
  static std::string
  time_point_to_string(const std::chrono::system_clock::time_point &tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
  }

  DatabaseTools() = delete;
};

#endif
#ifndef SESSIONTOOLS_H
#define SESSIONTOOLS_H

#include "../includes.h"

class SessionTools {
public:
  static std::string extractValue(const std::string &payload,
                                  const std::string &key) {
    std::string searchKey = R"(")" + key + R"(":")";
    auto pos = payload.find(searchKey);
    if (pos == std::string::npos)
      return {};

    pos += searchKey.length();
    auto end = payload.find('"', pos);
    if (end == std::string::npos)
      return {};

    return payload.substr(pos, end - pos);
  }

  SessionTools() = delete;
  ~SessionTools() = delete;
};

#endif
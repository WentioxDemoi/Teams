#ifndef SESSIONTOOLS_H
#define SESSIONTOOLS_H

#include "../includes.h"

class SessionTools {

public:
  static std::string extract_type(std::string payload) {
    auto pos = payload.find(R"("type":")");
    if (pos == std::string::npos)
      return {};
    pos += 8; // longueur de '"type":"'
    auto end = payload.find('"', pos);
    if (end == std::string::npos)
      return {};
    return payload.substr(pos, end - pos);
  }
  SessionTools() = delete;
  ~SessionTools() = delete;
};

#endif
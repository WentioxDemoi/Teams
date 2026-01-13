#ifndef SERVICESTOOLS_H
#define SERVICESTOOLS_H

#include "../includes.h"

class ServicesTools {

public:
  static std::string jsonEscape(const std::string &input)
{
    std::string out;
    out.reserve(input.size());

    for (char c : input)
    {
        switch (c)
        {
        case '\"': out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\b': out += "\\b";  break;
        case '\f': out += "\\f";  break;
        case '\n': out += "\\n";  break;
        case '\r': out += "\\r";  break;
        case '\t': out += "\\t";  break;
        default:
            if (static_cast<unsigned char>(c) < 0x20)
            {
                // caractères de contrôle -> ignorés ou échappés
            }
            else
            {
                out += c;
            }
        }
    }
    return out;
}

  ServicesTools() = delete;
  ~ServicesTools() = delete;
};

#endif
#ifndef NATSCLIENT_H
#define NATSCLIENT_H

#include "../includes.h"

class NatsClient {

public:
  static NatsClient &instance() {
    static NatsClient instance;
    return instance;
  }

  natsConnection *connection() const { return nc_; }

  void publish(const std::string &subject, const std::string &payload);
  void subscribe(const std::string &subject, natsMsgHandler cb, void *closure);

private:
  NatsClient();
  ~NatsClient();

  natsConnection *nc_;

  NatsClient(const NatsClient &) = delete;
  NatsClient &operator=(const NatsClient &) = delete;
};

#endif
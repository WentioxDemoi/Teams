#include "NatsClient.h"

NatsClient::NatsClient() {
  natsOptions *opts = nullptr;
  natsOptions_Create(&opts);

  // natsOptions_SetURL(opts, "nats://127.0.0.1:4222");
  natsOptions_SetURL(opts, "nats://nats:4222"); // Uniquement si container docker nats

natsStatus s = natsConnection_Connect(&nc_, opts);
if (s != NATS_OK) {
  // Message d'ERreur
}
  natsOptions_Destroy(opts);
}

NatsClient::~NatsClient() {
  if (nc_) {
    natsConnection_Destroy(nc_);
  }
}

void NatsClient::publish(const std::string &subject,
                         const std::string &payload) {
  natsConnection_Publish(nc_, subject.c_str(), payload.data(), payload.size());
}

void NatsClient::subscribe(const std::string &subject, natsMsgHandler cb,
                           void *closure) {
  natsSubscription *sub = nullptr;

  natsStatus s =
      natsConnection_Subscribe(&sub, nc_, subject.c_str(), cb, closure);

  if (s != NATS_OK) {
    throw std::runtime_error("Erreur lors du subscribe : " + subject);
  }
}
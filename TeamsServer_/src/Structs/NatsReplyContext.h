#include <nats/nats.h>
#include <string>

struct NatsReplyContext {
    natsConnection* nc;
    std::string reply_to;
};
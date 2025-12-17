#ifndef NATSCLIENT_H
#define NATSCLIENT_H

#include "../includes.h"

class NatsClient {

    public :
        static NatsClient &instance() {
            static NatsClient NatsClient_;
            return NatsClient_;
        }

        natsConnection *connection() const { return nc_; };
    private :
        NatsClient();
        ~NatsClient();

        natsConnection *nc_;

        NatsClient(const NatsClient &) = delete;
        NatsClient(NatsClient &&) = delete;
        NatsClient &operator=(const NatsClient &) = delete;
        NatsClient &operator=(NatsClient &&) = delete;
};

#endif
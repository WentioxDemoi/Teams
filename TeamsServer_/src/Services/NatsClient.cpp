#include "NatsClient.h"

NatsClient::NatsClient()
{
    nc_ = nullptr;
    natsStatus s = natsConnection_ConnectTo(&nc_, "nats://127.0.0.1:4222");
    if (s != NATS_OK)
        throw std::runtime_error(natsStatus_GetText(s));
}

NatsClient::~NatsClient()
{
    if (nc_)
        natsConnection_Destroy(nc_);
}
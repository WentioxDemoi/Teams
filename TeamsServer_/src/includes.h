#ifndef INCLUDES_H
#define INCLUDES_H

// ======================================================
// Standard Library
// ======================================================
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <optional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <random>
#include <cstdlib>
#include <chrono>

// ======================================================
// External Libraries
// ======================================================
#include <openssl/sha.h>
#include <pqxx/pqxx>
#include <nats/nats.h>
#include <argon2.h>

// ======================================================
// Boost / Asio
// ======================================================
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/thread_pool.hpp>

// ======================================================
// Namespaces & Aliases
// ======================================================
namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using udp = asio::ip::udp;
namespace ssl = asio::ssl;

#endif
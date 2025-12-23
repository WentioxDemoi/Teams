#ifndef INCLUDES_H
#define INCLUDES_H

// ======================================================
// Standard Library
// ======================================================
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <random>
#include <sstream>
#include <string>

// ======================================================
// External Libraries
// ======================================================
#include <argon2.h>
#include <nats/nats.h>
#include <openssl/sha.h>
#include <pqxx/pqxx>

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
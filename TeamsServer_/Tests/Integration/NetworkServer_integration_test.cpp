#include <gtest/gtest.h>

#include <future>
#include <thread>

#include "../Mocks/AuthServiceMock.h"
#include "../Mocks/ContactServiceMock.h"
#include "../Mocks/ContactSessionRegistryMock.h"
#include "../Mocks/MessageServiceMock.h"
#include "../Mocks/MessageSessionRegistryMock.h"
#include "../Mocks/WebRTCRegistryMock.h"
#include "../Mocks/WebRTCServiceMock.h"
#include "Server/TcpListenerAuth.h"
#include "Server/TcpListenerContact.h"
#include "Server/TcpListenerMessage.h"
#include "Server/TcpListenerWebRTC.h"

using ::testing::_;
using ::testing::HasSubstr;
using ::testing::Return;

namespace {

constexpr const char* certificatePath = "../certs/server.crt";
constexpr const char* keyPath = "../certs/server.key";

void configureServerContext(ssl::context& context) {
  context.use_certificate_chain_file(certificatePath);
  context.use_private_key_file(keyPath, ssl::context::pem);
}

struct TlsClient {
  std::unique_ptr<ssl::context> context;
  std::unique_ptr<ssl::stream<tcp::socket>> stream;
};

TlsClient connectTls(asio::io_context& io, unsigned short port) {
  auto context = std::make_unique<ssl::context>(ssl::context::tls_client);
  context->set_verify_mode(ssl::verify_none);

  auto stream = std::make_unique<ssl::stream<tcp::socket>>(io, *context);
  stream->lowest_layer().connect(tcp::endpoint(asio::ip::address_v4::loopback(), port));
  stream->handshake(ssl::stream_base::client);
  return {std::move(context), std::move(stream)};
}

std::string sendAndRead(ssl::stream<tcp::socket>& stream, const std::string& payload) {
  asio::write(stream, asio::buffer(payload));
  asio::streambuf buffer;
  asio::read_until(stream, buffer, '\n');
  std::istream input(&buffer);
  std::string response;
  std::getline(input, response);
  return response;
}

struct IoGuard {
  explicit IoGuard(asio::io_context& io) : io_(io), thread_([this] { io_.run(); }) {}

  ~IoGuard() {
    io_.stop();
    if (thread_.joinable()) thread_.join();
  }

  asio::io_context& io_;
  std::thread thread_;
};

User authenticatedUser() {
  User user;
  user.uuid = "u1";
  user.email = "user@example.com";
  return user;
}

}  // namespace

TEST(NetworkServerIntegrationTest, AuthListenerPerformsTlsHandshakeAndDelegatesLogin) {
  asio::io_context io;
  ssl::context serverContext(ssl::context::tls_server);
  configureServerContext(serverContext);
  auto authService = std::make_shared<MockAuthService>();
  EXPECT_CALL(*authService, loginUser(testing::Field(&User::email, "user@example.com")))
      .WillOnce(Return(authenticatedUser()));
  auto handler = std::make_shared<AuthHandler>(authService);
  auto listener = std::make_unique<TcpListenerAuth>(io, serverContext,
                                                    tcp::endpoint(tcp::v4(), 19080), handler);
  IoGuard ioGuard(io);

  auto client = connectTls(io, 19080);
  const auto response = sendAndRead(
      *client.stream, R"({"type":"login","email":"user@example.com","password":"secret"})");

  EXPECT_THAT(response, HasSubstr("login_response"));
}

TEST(NetworkServerIntegrationTest, ContactListenerAuthenticatesAndRegistersSession) {
  asio::io_context io;
  ssl::context serverContext(ssl::context::tls_server);
  configureServerContext(serverContext);
  auto authService = std::make_shared<MockAuthService>();
  EXPECT_CALL(*authService, validateToken("token-1")).WillOnce(Return(authenticatedUser()));

  auto contactService = std::make_unique<MockContactService>();
  auto* contactServiceMock = contactService.get();
  EXPECT_CALL(*contactServiceMock, updateStatus("u1", HasSubstr("Online")))
      .WillOnce(Return(std::nullopt));
  EXPECT_CALL(*contactServiceMock, updateStatus("u1", HasSubstr("Offline")))
      .WillOnce(Return(std::nullopt));
  auto contactHandler = std::make_shared<ContactHandler>(std::move(contactService));

  auto registry = std::make_shared<MockContactSessionRegistry>();
  std::promise<void> registered;
  auto registeredFuture = registered.get_future();
  EXPECT_CALL(*registry, registerContactSession("u1", _))
      .WillOnce([&registered](const auto&, const auto&) { registered.set_value(); });
  EXPECT_CALL(*registry, unregisterContactSession("u1"));

  auto listener = std::make_unique<TcpListenerContact>(
      io, serverContext, tcp::endpoint(tcp::v4(), 19084), contactHandler, registry, authService);
  IoGuard ioGuard(io);

  auto client = connectTls(io, 19084);
  const auto response = sendAndRead(*client.stream, R"({"token":"token-1"})");

  EXPECT_EQ(response, R"({"type":"auth_success"})");
  EXPECT_EQ(registeredFuture.wait_for(std::chrono::seconds(2)), std::future_status::ready);
}

TEST(NetworkServerIntegrationTest, MessageListenerAuthenticatesAndRegistersSession) {
  asio::io_context io;
  ssl::context serverContext(ssl::context::tls_server);
  configureServerContext(serverContext);
  auto authService = std::make_shared<MockAuthService>();
  EXPECT_CALL(*authService, validateToken("token-1")).WillOnce(Return(authenticatedUser()));
  auto messageService = std::make_unique<MockMessageService>();
  auto messageHandler = std::make_shared<MessageHandler>(std::move(messageService));

  auto registry = std::make_shared<MockMessageSessionRegistry>();
  std::promise<void> registered;
  auto registeredFuture = registered.get_future();
  std::promise<void> unregistered;
  auto unregisteredFuture = unregistered.get_future();
  EXPECT_CALL(*registry, registerMessageSession("u1", _))
      .WillOnce([&registered](const auto&, const auto&) { registered.set_value(); });
  EXPECT_CALL(*registry, unregisterMessageSession("u1")).WillOnce([&unregistered](const auto&) {
    unregistered.set_value();
  });

  auto listener = std::make_unique<TcpListenerMessage>(
      io, serverContext, tcp::endpoint(tcp::v4(), 19082), messageHandler, registry, authService);
  IoGuard ioGuard(io);

  auto client = connectTls(io, 19082);
  asio::write(*client.stream, asio::buffer(R"({"token":"token-1"})"));

  EXPECT_EQ(registeredFuture.wait_for(std::chrono::seconds(2)), std::future_status::ready);

  boost::system::error_code ec;
  client.stream->lowest_layer().shutdown(tcp::socket::shutdown_both, ec);
  client.stream->lowest_layer().close(ec);

  EXPECT_EQ(unregisteredFuture.wait_for(std::chrono::seconds(2)), std::future_status::ready);
}

TEST(NetworkServerIntegrationTest, WebRTCListenerAuthenticatesAndRegistersSession) {
  asio::io_context io;
  ssl::context serverContext(ssl::context::tls_server);
  configureServerContext(serverContext);
  auto authService = std::make_shared<MockAuthService>();
  EXPECT_CALL(*authService, validateToken("token-1")).WillOnce(Return(authenticatedUser()));
  auto webRTCService = std::make_unique<MockWebRTCService>();
  auto webRTCHandler = std::make_shared<WebRTCHandler>(std::move(webRTCService));

  auto registry = std::make_shared<MockWebRTCRegistry>();
  std::promise<void> registered;
  auto registeredFuture = registered.get_future();
  std::promise<void> unregistered;
  auto unregisteredFuture = unregistered.get_future();
  EXPECT_CALL(*registry, register_session("u1", _))
      .WillOnce([&registered](const auto&, const auto&) { registered.set_value(); });
  EXPECT_CALL(*registry, unregister_session("u1")).WillOnce([&unregistered](const auto&) {
    unregistered.set_value();
  });

  auto listener = std::make_unique<TcpListenerWebRTC>(
      io, serverContext, tcp::endpoint(tcp::v4(), 19083), registry, webRTCHandler, authService);
  IoGuard ioGuard(io);

  auto client = connectTls(io, 19083);
  asio::write(*client.stream, asio::buffer(std::string(R"({"token":"token-1"})") + "\n"));

  EXPECT_EQ(registeredFuture.wait_for(std::chrono::seconds(2)), std::future_status::ready);

  boost::system::error_code ec;
  client.stream->lowest_layer().shutdown(tcp::socket::shutdown_both, ec);
  client.stream->lowest_layer().close(ec);

  EXPECT_EQ(unregisteredFuture.wait_for(std::chrono::seconds(2)), std::future_status::ready);
}
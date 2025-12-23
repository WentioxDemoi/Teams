#include "AuthSession.h"

void AuthSession::start() {
  auto self = shared_from_this();
  stream_.async_handshake(ssl::stream_base::server,
                          [this, self](boost::system::error_code ec) {
                            if (!ec) {
                              do_read();
                            } else {
                              handle_error("AuthSession Handshake: ", ec);
                            }
                          });
}

void AuthSession::do_read() {
  auto self = shared_from_this();
  stream_.async_read_some(
      asio::buffer(buffer_),
      [this, self](boost::system::error_code ec, std::size_t bytes) {
        if (!ec) {
          std::string payload(buffer_.data(), bytes);
          std::cout << "Message : " << payload << std::endl;
          handle_type(payload);
          do_read();
        } else {
          handle_error("AuthSession Read: ", ec);
        }
      });
}

void AuthSession::handle_type(std::string payload) {
  std::string type = SessionTools::extractValue(payload, "type");
  std::cout << "type : " << type << std::endl;
  if (type.empty())
    return;

  if (type == "login") {
    handle_login(payload);
  } else if (type == "register") {
    handle_register(payload);
  } else if (type == "validate_token") {
    handle_token(payload);
  } else {
    std::cerr << "[AuthSession] Unknown message type\n";
  }
}

void AuthSession::handle_login(std::string payload) {

  std::cout << "Login" << std::endl;
  natsConnection *nc = NatsClient::instance().connection();
  if (!nc) {
    std::cout << "no connection with nats" << std::endl;
    return;
  }

  char *inbox = nullptr;
  natsInbox_Create(&inbox);

  natsSubscription *sub = nullptr;
  std::cout << "BoxLogin : " << inbox << std::endl;

  natsConnection_Subscribe(
      &sub, nc, inbox,
      [](natsConnection *, natsSubscription *sub, natsMsg *msg, void *closure) {
        auto *self = static_cast<AuthSession *>(closure);

        std::string response(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));

        self->handle_login_response(response);

        natsMsg_Destroy(msg);
        natsSubscription_Unsubscribe(sub);
        natsSubscription_Destroy(sub);
      },
      this);

  natsConnection_PublishRequestString(nc, "auth.login", inbox, payload.c_str());
}

void AuthSession::handle_register(std::string payload) {
  std::cout << "Register" << std::endl;
  natsConnection *nc = NatsClient::instance().connection();
  if (!nc) {
    std::cout << "no connection with nats" << std::endl;
    return;
  }

  char *inbox = nullptr;
  natsInbox_Create(&inbox);

  natsSubscription *sub = nullptr;
  std::cout << "BoxRegister : " << inbox << std::endl;

  natsConnection_Subscribe(
      &sub, nc, inbox,
      [](natsConnection *, natsSubscription *sub, natsMsg *msg, void *closure) {
        auto *self = static_cast<AuthSession *>(closure);

        std::string response(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));

        self->handle_register_response(response);

        natsMsg_Destroy(msg);
        natsSubscription_Unsubscribe(sub);
        natsSubscription_Destroy(sub);
      },
      this);

  natsConnection_PublishRequestString(nc, "auth.register", inbox,
                                      payload.c_str());
}

void AuthSession::handle_register_response(std::string payload) {
  auto self = shared_from_this();
  std::string response = payload; // tmp
  std::cout << "aller" + response << std::endl;

  asio::post(stream_.get_executor(), [this, self, response]() {
    asio::async_write(
        stream_, asio::buffer(response + "\n"),
        [this, self](boost::system::error_code ec, std::size_t bytes) {
          std::cout << "aller2" << std::endl;
          if (ec) {
            std::cout << "aller3" << std::endl;
            handle_error("AuthSession Write: ", ec);
          }
        });
  });
}

void AuthSession::handle_login_response(std::string payload) {
  auto self = shared_from_this();
  std::string response = payload;
  asio::post(stream_.get_executor(), [this, self, response]() {
    asio::async_write(
        stream_, asio::buffer(response + "\n"),
        [this, self](boost::system::error_code ec, std::size_t bytes) {
          if (ec) {
            handle_error("AuthSession Write: ", ec);
          }
        });
  });
}

void AuthSession::handle_token(std::string payload) {
  // validation du token dans db_pool
}
#include "Handler.h"

Handler::Handler() : worker_pool_(Config::instance().worker_pool_size()) {
  authHandler_ = std::make_unique<AuthHandler>(worker_pool_);
}

void Handler::handle_type(std::string payload, ResponseCallback respond) {
  std::string type = HandlerTools::extractValue(payload, "type");
  std::string result;
  std::cout << "type : " << type << std::endl;
  if (type.empty())
    return;

  if (type == "login") {
    authHandler_->handle_login(payload, respond);
  } else if (type == "register") {
    authHandler_->handle_register(payload, respond);
  } else if (type == "validate_token") {
    authHandler_->handle_token(payload, respond);
  } else {
    std::cerr << "[AuthSession] Unknown message type\n";
  }
}
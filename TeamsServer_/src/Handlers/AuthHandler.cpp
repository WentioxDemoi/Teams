#include "AuthHandler.h"

void AuthHandler::handle_login(std::string payload, ResponseCallback respond) {
  auto email = HandlerTools::extractValue(payload, "email");
  auto password = HandlerTools::extractValue(payload, "password");

  asio::post(worker_pool_, [this, email, password, respond]() {
    try {
      auto response = authService_->loginUser(email, password);
      std::string result;
      if (!response.has_value()) {
        result =
            R"({"type":"login_response","error":"Login failed: invalid username or password."})";
      } else {
        result =
            ResponseFormater::format_user_response("login_response", *response);
      }

      respond(result);

    } catch (const std::exception &e) {
      std::cerr << "[AuthSession] Login error: " << e.what() << "\n";

      std::string error =
          R"({"type":"login_response","error":"Login failed: server error"})";
      respond(error);
    }
  });
}

void AuthHandler::handle_register(std::string payload,
                                  ResponseCallback respond) {
  auto email = HandlerTools::extractValue(payload, "email");
  auto username = HandlerTools::extractValue(payload, "username");
  auto password = HandlerTools::extractValue(payload, "password");

  asio::post(worker_pool_, [this, email, username, password, respond]() {
    try {
      auto response = authService_->registerUser(username, email, password);
      std::string result;
      if (!response.has_value()) {
        result =
            R"({"type":"register_response","error":"Registration failed: username may already exist or invalid data."})";
      } else {
        result = ResponseFormater::format_user_response("register_response",
                                                        *response);
      }

      respond(result);

    } catch (const std::exception &e) {
      std::cerr << "[AuthSession] Register error: " << e.what() << "\n";
      std::string error =
          R"({"type":"register_response","error":"Register failed: server error"})";
      respond(error);
    }
  });
}

void AuthHandler::handle_token(std::string payload, ResponseCallback respond) {
  auto token = HandlerTools::extractValue(payload, "token");

  asio::post(worker_pool_, [this, token, respond]() {
    try {
      auto response = authService_->validateToken(token);
      std::string result;

      if (!response.has_value()) {
        result =
            R"({"type":"validate_token_response","error":"Login failed: invalid token."})";
      } else {
        result = ResponseFormater::format_user_response(
            "validate_token_response", *response);
      }
      respond(result);

    } catch (const std::exception &e) {

      std::string error =
          R"({"type":"validate_token_response","error":"Token validation failed: server error"})";
      respond(error);
    }
  });
}

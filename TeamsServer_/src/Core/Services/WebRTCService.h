#ifndef WEBRTCSERVICE_H
#define WEBRTCSERVICE_H

#include "../../Utils/Crypto.h"
#include "../Repositories/UserRepository.h"

/**
 * @class WebRTCService
 * @brief 
 *
 */
class WebRTCService {
public:
  WebRTCService(std::unique_ptr<UserRepository> userRepo)
      : userRepo_(std::move(userRepo)), config_(Config::instance()) {};
  virtual ~WebRTCService() = default;

  std::optional<User> find_by_token(const std::string& token) {
    return userRepo_->find_by_token(token);
}


private:
  std::unique_ptr<UserRepository> userRepo_;
  Config &config_;
};

#endif
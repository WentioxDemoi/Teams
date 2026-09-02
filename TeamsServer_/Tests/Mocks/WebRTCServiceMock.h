#ifndef WEBRTCSERVICEMOCK_H
#define WEBRTCSERVICEMOCK_H

#include "Core/Services/IWebRTCService.h"
#include <gmock/gmock.h>

class MockWebRTCService : public IWebRTCService {
 public:
  MOCK_METHOD(std::optional<std::string>, requestCall, (const std::string &callerUuid, const std::string &targetUuid), (override));
  MOCK_METHOD(bool, acceptCall, (const std::string &calleeUuid, const std::string &targetUuid), (override));
  MOCK_METHOD(bool, rejectCall, (const std::string &calleeUuid, const std::string &targetUuid), (override));
  MOCK_METHOD(bool, cancelCall, (const std::string &callerUuid, const std::string &targetUuid), (override));
  MOCK_METHOD(bool, hangupCall, (const std::string &senderUuid, const std::string &targetUuid), (override));
  MOCK_METHOD(bool, relaySignaling, (const std::string &senderUuid, const std::string &targetUuid, const std::string &payload), (override));
  MOCK_METHOD(bool, cameraEnabledChange, (const std::string &senderUuid, const std::string &targetUuid, const std::string &payload), (override));
};

#endif
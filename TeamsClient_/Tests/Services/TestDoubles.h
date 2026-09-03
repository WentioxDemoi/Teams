#ifndef SERVICE_TEST_DOUBLES_H
#define SERVICE_TEST_DOUBLES_H

#include "Interfaces/ICallService.h"
#include "Interfaces/ILocalUserService.h"
#include "Interfaces/IMessageService.h"
#include "Network/INetworkService.h"
#include "P2P/IWebRTCService.h"
#include "Repositories/IMessageRepository.h"
#include "Repositories/IUserRepository.h"
#include "Utils/Interfaces/ITokenManager.h"

class FakeNetwork : public INetworkService {
  Q_OBJECT
 public:
  explicit FakeNetwork(QObject* parent = nullptr) : INetworkService(parent) {}
  void send(const QJsonObject& payload) override { sentPayloads.append(payload); }
  void disconnectFromServer() override { disconnected = true; }
  QList<QJsonObject> sentPayloads;
  bool disconnected = false;
};

class FakeTokenManager : public ITokenManager {
  Q_OBJECT
 public:
  explicit FakeTokenManager(QObject* parent = nullptr) : ITokenManager(parent) {}
  bool writeToken(const QString& value) override { token = value; return true; }
  bool deleteToken() override { token.clear(); deleted = true; return true; }
  bool readToken() override { return !token.isEmpty(); }
  bool deleted = false;
};

class MockMessageRepository : public IMessageRepository {
  Q_OBJECT
 public:
  explicit MockMessageRepository(QObject* parent = nullptr) : IMessageRepository(parent) {}
  QList<Message> findAll() override { ++findAllCalls; return storedMessages; }
  bool save(const Message& message) override {
    ++saveCalls;
    if (saveResult) storedMessages.append(message);
    return saveResult;
  }
  bool removeAll() override { ++removeAllCalls; return removeAllResult; }
  QList<Message> storedMessages;
  bool saveResult = true;
  bool removeAllResult = true;
  int findAllCalls = 0;
  int saveCalls = 0;
  int removeAllCalls = 0;
};

class MockUserRepository : public IUserRepository {
  Q_OBJECT
 public:
  explicit MockUserRepository(QObject* parent = nullptr) : IUserRepository(parent) {}
  bool save(const User& user) override { ++saveCalls; savedUsers.append(user); return saveResult; }
  QList<User> findAll() override { ++findAllCalls; return storedUsers; }
  bool removeAll() override { ++removeAllCalls; return removeAllResult; }
  bool updateLastReadAt(const QString& uuid, const QString& timestamp) override {
    ++updateLastReadAtCalls;
    updatedUuid = uuid;
    updatedTimestamp = timestamp;
    return updateLastReadAtResult;
  }
  QList<User> storedUsers;
  QList<User> savedUsers;
  QString updatedUuid;
  QString updatedTimestamp;
  bool saveResult = true;
  bool removeAllResult = true;
  bool updateLastReadAtResult = true;
  int saveCalls = 0;
  int findAllCalls = 0;
  int removeAllCalls = 0;
  int updateLastReadAtCalls = 0;
};

class FakeLocalUserService : public ILocalUserService {
  Q_OBJECT
 public:
  explicit FakeLocalUserService(QObject* parent = nullptr) : ILocalUserService(parent) {}
  void saveLocalUser(const User& user) override { savedUser = user; emit localUserSaved(user); }
  void deleteAll() override { deleted = true; }
  User savedUser;
  bool deleted = false;
};

class FakeMessageService : public IMessageService {
  Q_OBJECT
 public:
  explicit FakeMessageService(QObject* parent = nullptr) : IMessageService(parent) {}
  void loadConversationsFromServer() override { ++loadCalls; loadCalled = true; }
  void sendMessage(const Message& message) override { sentMessage = message; ++sendCalls; sendCalled = true; }
  void disconnectFromServer() override { ++disconnectCalls; disconnected = true; }
  void deleteAll() override { ++deleteCalls; deleteCalled = true; }
  Message sentMessage;
  bool loadCalled = false;
  bool sendCalled = false;
  bool disconnected = false;
  bool deleteCalled = false;
  int loadCalls = 0;
  int sendCalls = 0;
  int disconnectCalls = 0;
  int deleteCalls = 0;
};

class FakeCallService : public ICallService {
  Q_OBJECT
 public:
  explicit FakeCallService(QObject* parent = nullptr) : ICallService(parent) {}
  void startCall(const QString& uuid, const QString& username) override { callUuid = uuid; callUsername = username; ++startCalls; startCalled = true; }
  void hangup() override { ++hangupCalls; hangupCalled = true; }
  void acceptCall(const QString& username) override { acceptedUsername = username; ++acceptCalls; acceptCalled = true; }
  void rejectCall() override { ++rejectCalls; rejectCalled = true; }
  void cameraEnabledChanged(bool enabled) override { cameraEnabled = enabled; ++cameraCalls; cameraCalled = true; }
  void disconnectFromServer() override { ++disconnectCalls; disconnected = true; }
  QString callUuid;
  QString callUsername;
  QString acceptedUsername;
  bool cameraEnabled = false;
  bool startCalled = false;
  bool hangupCalled = false;
  bool acceptCalled = false;
  bool rejectCalled = false;
  bool cameraCalled = false;
  bool disconnected = false;
  int startCalls = 0;
  int hangupCalls = 0;
  int acceptCalls = 0;
  int rejectCalls = 0;
  int cameraCalls = 0;
  int disconnectCalls = 0;
};

class FakeWebRTCService : public IWebRTCService {
  Q_OBJECT
 public:
  explicit FakeWebRTCService(QObject* parent = nullptr) : IWebRTCService(parent) {}
  void setCallBacks(std::function<void(const std::string&)> offer,
                    std::function<void(const std::string&)> answer,
                    std::function<void(const std::string&, const std::string&, int)> ice,
                    std::function<void(bool)> connected) override {
    onOffer = offer; onAnswer = answer; onIce = ice; onConnected = connected;
  }
  void startCall() override { startCalled = true; }
  void acceptCall() override { acceptCalled = true; }
  void hangup() override { hangupCalled = true; }
  void onRemoteOffer(QString value) override { remoteOffer = value; }
  void onRemoteAnswer(QString value) override { remoteAnswer = value; }
  void onRemoteIce(QString candidate, QString mid, int index) override {
    remoteCandidate = candidate; remoteMid = mid; remoteIndex = index;
  }
  std::function<void(const std::string&)> onOffer;
  std::function<void(const std::string&)> onAnswer;
  std::function<void(const std::string&, const std::string&, int)> onIce;
  std::function<void(bool)> onConnected;
  QString remoteOffer;
  QString remoteAnswer;
  QString remoteCandidate;
  QString remoteMid;
  int remoteIndex = -1;
  bool startCalled = false;
  bool acceptCalled = false;
  bool hangupCalled = false;
};

#endif
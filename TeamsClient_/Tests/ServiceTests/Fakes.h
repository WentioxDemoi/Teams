#ifndef FAKES_H
#define FAKES_H

#include "../../../src/Services/Interfaces/IAuthNetworkService.h"
#include "../../../src/Utils/Interfaces/ITokenManager.h"
#include "../../../src/Services/Interfaces/IUserService.h"
#include "User.h"
#include "includes.h"
#include <QtTest>

// FakeAuthNetworkService.h
class FakeAuthNetworkService : public IAuthNetworkService {
  Q_OBJECT
public:
  QString lastEmail, lastPassword;
  QString lastRegisterEmail, lastRegisterUsername, lastRegisterPassword;
  QByteArray lastServerResponse;
  QString lastValidatedToken;

  void loginUser(const QString &email, const QString &password) override {
    lastEmail = email;
    lastPassword = password;
  }

  void registerUser(const QString &email, const QString &username,
                    const QString &password) override {
    lastRegisterEmail = email;
    lastRegisterUsername = username;
    lastRegisterPassword = password;
  }

  void handleServerResponse(const QByteArray &data) override {
    lastServerResponse = data;
    // Simuler une réponse simple JSON
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject()) {
      QJsonObject obj = doc.object();
      if (obj.contains("success") && obj["success"].toBool()) {
        User u;
        u.setId(1);
        u.setEmail("test@example.com");
        u.setUsername("tester");
        u.setStatus("active");
        u.setUuid("uuid-123");
        emit authSuccess(u);
      } else {
        emit authError(obj.value("error").toString());
      }
    }
  }

  void validateToken(const QString &value) override {
    lastValidatedToken = value;
  }

  void disconnectFromServer() override {}

private:
  void sendRequest(const QJsonObject &payload) override {}
  void sendPendingPayload() override {}
};

// FakeUserService complet
class FakeUserServiceFull : public IUserService {
  Q_OBJECT
public:
  bool saveCalled = false;
  bool deleteCalled = false;
  bool deleteAllCalled = false;
  QString deletedUuid;

  void saveUser(const User &user) override {
    saveCalled = true;
    emit userSaved(user);
  }

  void deleteUser(QString uuid) override {
    deleteCalled = true;
    deletedUuid = uuid;
    emit userDeleted(uuid);
  }

  void deleteAll() override { deleteAllCalled = true; }
};

class FakeTokenManager : public ITokenManager {
  Q_OBJECT
public:
  QString token;
  bool writeCalled = false;
  bool deleteCalled = false;

  FakeTokenManager(QObject *parent = nullptr) : ITokenManager(parent) {}

  bool writeToken(const QString &value) override {
    token = value;
    writeCalled = true;
    return true;
  }

  bool deleteToken() override {
    token.clear();
    deleteCalled = true;
    return true;
  }

  bool readToken() override { return !token.isEmpty(); }
};

#endif
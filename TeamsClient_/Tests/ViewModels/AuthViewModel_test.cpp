#include "AuthViewModel_test.h"

#include "Core/State/SessionState.h"
#include "Core/State/UserState.h"
#include "ViewModels/AuthViewModel.h"
#include "../Services/TestDoubles.h"

#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

void AuthViewModelTest::startDelegatesTokenLogin() {
  FakeAuthService auth;
  UserState userState;
  SessionState sessionState;
  AuthViewModel viewModel(&auth, &userState, &sessionState);

  viewModel.start();

  QCOMPARE(auth.tokenLoginCalls, 1);
}

void AuthViewModelTest::loginRejectsMissingCredentials() {
  FakeAuthService auth;
  UserState userState;
  SessionState sessionState;
  AuthViewModel viewModel(&auth, &userState, &sessionState);
  QSignalSpy errorSpy(&viewModel, &AuthViewModel::authError);

  viewModel.loginUser({}, "password");
  viewModel.loginUser("alice@example.com", {});

  QCOMPARE(auth.loginCalls, 0);
  QCOMPARE(errorSpy.count(), 2);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Username and password required"));
}

void AuthViewModelTest::loginDelegatesValidCredentials() {
  FakeAuthService auth;
  UserState userState;
  SessionState sessionState;
  AuthViewModel viewModel(&auth, &userState, &sessionState);

  viewModel.loginUser("alice@example.com", "password");

  QCOMPARE(auth.loginCalls, 1);
  QCOMPARE(auth.loginEmail, QStringLiteral("alice@example.com"));
  QCOMPARE(auth.loginPassword, QStringLiteral("password"));
}

void AuthViewModelTest::registerRejectsMissingFields() {
  FakeAuthService auth;
  UserState userState;
  SessionState sessionState;
  AuthViewModel viewModel(&auth, &userState, &sessionState);
  QSignalSpy errorSpy(&viewModel, &AuthViewModel::authError);

  viewModel.registerUser({}, "Martin", "alice@example.com", "password");

  QCOMPARE(auth.registerCalls, 0);
  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("All fields are required"));
}

void AuthViewModelTest::registerRejectsInvalidEmail() {
  FakeAuthService auth;
  UserState userState;
  SessionState sessionState;
  AuthViewModel viewModel(&auth, &userState, &sessionState);
  QSignalSpy errorSpy(&viewModel, &AuthViewModel::authError);

  viewModel.registerUser("Alice", "Martin", "invalid-email", "password");

  QCOMPARE(auth.registerCalls, 0);
  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(errorSpy.at(0).at(0).toString(), QStringLiteral("Invalid email"));
}

void AuthViewModelTest::registerDelegatesValidFields() {
  FakeAuthService auth;
  UserState userState;
  SessionState sessionState;
  AuthViewModel viewModel(&auth, &userState, &sessionState);

  viewModel.registerUser("Alice", "Martin", "alice@example.com", "password");

  QCOMPARE(auth.registerCalls, 1);
  QCOMPARE(auth.registerFirstName, QStringLiteral("Alice"));
  QCOMPARE(auth.registerLastName, QStringLiteral("Martin"));
  QCOMPARE(auth.registerEmail, QStringLiteral("alice@example.com"));
  QCOMPARE(auth.registerPassword, QStringLiteral("password"));
}

void AuthViewModelTest::forwardsAuthSignals() {
  FakeAuthService auth;
  UserState userState;
  SessionState sessionState;
  AuthViewModel viewModel(&auth, &userState, &sessionState);
  QSignalSpy errorSpy(&viewModel, &AuthViewModel::authError);
  QSignalSpy successSpy(&viewModel, &AuthViewModel::authSuccess);
  QSignalSpy noTokenSpy(&viewModel, &AuthViewModel::noTokenFound);
  const User user("alice@example.com", "Alice", "Martin", "Online", true, {}, "user-1");

  emit auth.authError("invalid credentials");
  emit auth.authSuccess(user);
  emit auth.noTokenFound();

  QCOMPARE(errorSpy.count(), 1);
  QCOMPARE(successSpy.count(), 1);
  QCOMPARE(noTokenSpy.count(), 1);
  QCOMPARE(successSpy.at(0).at(0).value<User>().uuid(), QStringLiteral("user-1"));
}
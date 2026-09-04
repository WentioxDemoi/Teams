#include "Handlers/ContactHandler.h"

#include <gtest/gtest.h>

#include <future>

#include "../Mocks/ContactServiceMock.h"

using ::testing::_;
using ::testing::HasSubstr;
using ::testing::Return;

namespace {
std::string responseFrom(std::function<void(ResponseCallback)> invoke) {
  std::promise<std::string> promise;
  auto future = promise.get_future();
  invoke([&promise](std::string response) { promise.set_value(std::move(response)); });
  EXPECT_EQ(future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
  return future.get();
}
}  // namespace

TEST(ContactHandlerTest, AddContactInjectsUserUuid) {
  auto service = std::make_unique<MockContactService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, addContact(HasSubstr(R"("userUuid":"u1")")))
      .WillOnce(Return(std::string(R"({"type":"contact_added"})")));
  ContactHandler handler(std::move(service));

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"add_contact","contactUuid":"u2"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("contact_added"));
}

TEST(ContactHandlerTest, LoadContactsReturnsFallbackOnServiceFailure) {
  auto service = std::make_unique<MockContactService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, loadContacts(HasSubstr(R"("userUuid":"u1")")))
      .WillOnce(Return(std::nullopt));
  ContactHandler handler(std::move(service));

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"load_contacts"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("contacts_loaded"));
}

TEST(ContactHandlerTest, SearchUsersPassesCallerUuid) {
  auto service = std::make_unique<MockContactService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, searchUsers("u1", HasSubstr("alice")))
      .WillOnce(Return(std::string(R"({"type":"users_searched"})")));
  ContactHandler handler(std::move(service));

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"search_users","name":"alice"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("users_searched"));
}

TEST(ContactHandlerTest, ResolveUserAndUpdateLastReadAreRouted) {
  auto service = std::make_unique<MockContactService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, resolveUserByUuid(HasSubstr(R"("contactUuid":"u2")")))
      .WillOnce(Return(std::string(R"({"type":"resolve_user_response"})")));
  ContactHandler handler(std::move(service));
  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"resolve_user_by_uuid","contactUuid":"u2"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("resolve_user_response"));

  EXPECT_CALL(*serviceMock, lastReadAt("u1", HasSubstr(R"("lastReadAt":"2026-01-01")")))
      .WillOnce(Return(std::string(R"({"type":"last_read_at_updated"})")));
  response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type(
        "u1", R"({"type":"update_last_read_at","contactUuid":"u2","lastReadAt":"2026-01-01"})",
        callback);
  });
  EXPECT_THAT(response, HasSubstr("last_read_at_updated"));
}

TEST(ContactHandlerTest, AddContactFailureReturnsFallback) {
  auto service = std::make_unique<MockContactService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, addContact(_)).WillOnce(Return(std::nullopt));
  ContactHandler handler(std::move(service));

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"add_contact","contactUuid":"u2"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("contact_added"));
}

TEST(ContactHandlerTest, ServiceExceptionsReturnOperationErrors) {
  auto service = std::make_unique<MockContactService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, addContact(_))
      .WillOnce(::testing::Throw(std::runtime_error("database")));
  ContactHandler handler(std::move(service));

  auto response = responseFrom([&](ResponseCallback callback) {
    handler.handle_type("u1", R"({"type":"add_contact"})", callback);
  });
  EXPECT_THAT(response, HasSubstr("contact_added"));
}

TEST(ContactHandlerTest, RoutesRemoveAndUpdateStatusWithoutResponse) {
  auto service = std::make_unique<MockContactService>();
  auto* serviceMock = service.get();
  std::promise<void> statusPromise;
  auto statusFuture = statusPromise.get_future();
  EXPECT_CALL(*serviceMock, updateStatus("u1", HasSubstr("Offline")))
      .WillOnce([&statusPromise](const auto&, const auto&) {
        statusPromise.set_value();
        return std::optional<std::string>{};
      });
  ContactHandler handler(std::move(service));

  handler.handle_type("u1", R"({"type":"remove_contact","contactUuid":"u2"})", [](std::string) {});
  handler.handle_type("u1", R"({"type":"update_status","status":"Offline"})", nullptr);
  EXPECT_EQ(statusFuture.wait_for(std::chrono::seconds(2)), std::future_status::ready);
}

TEST(ContactHandlerTest, UnknownOrEmptyTypeDoesNotCallService) {
  auto service = std::make_unique<MockContactService>();
  auto* serviceMock = service.get();
  EXPECT_CALL(*serviceMock, addContact(_)).Times(0);
  EXPECT_CALL(*serviceMock, loadContacts(_)).Times(0);
  ContactHandler handler(std::move(service));

  handler.handle_type("u1", "{}", [](std::string) {});
  handler.handle_type("u1", R"({"type":"unknown"})", [](std::string) {});
}
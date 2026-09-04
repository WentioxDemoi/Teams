#include "Core/Services/MessageService.h"

#include <gtest/gtest.h>

#include "../Mocks/MessageRepositoryMock.h"
#include "../Mocks/MessageSessionRegistryMock.h"

using ::testing::_;
using ::testing::Return;

namespace {

std::unique_ptr<MessageService> makeService(MockMessageRepository*& rawMessageRepo,
                                            std::shared_ptr<MockMessageSessionRegistry>& registry) {
  auto messageRepo = std::make_unique<MockMessageRepository>();
  rawMessageRepo = messageRepo.get();
  registry = std::make_shared<MockMessageSessionRegistry>();
  return std::make_unique<MessageService>(std::move(messageRepo), registry);
}

}  // namespace

// ---------------------------------------------------------------------------
// sendMessage
// ---------------------------------------------------------------------------

TEST(MessageServiceTest, SendMessage_MissingContent_ReturnsNulloptAndSkipsRepo) {
  MockMessageRepository* messageRepo;
  std::shared_ptr<MockMessageSessionRegistry> registry;
  auto svc = makeService(messageRepo, registry);

  EXPECT_CALL(*messageRepo, save(_)).Times(0);
  EXPECT_CALL(*registry, sendMessage(_, _)).Times(0);

  auto res = svc->sendMessage(R"({"senderUuid":"u1","receiverUuid":"u2"})");
  EXPECT_FALSE(res.has_value());
}

TEST(MessageServiceTest, SendMessage_SaveFails_ReturnsNullopt) {
  MockMessageRepository* messageRepo;
  std::shared_ptr<MockMessageSessionRegistry> registry;
  auto svc = makeService(messageRepo, registry);

  EXPECT_CALL(*messageRepo, save(_)).WillOnce(Return(false));
  EXPECT_CALL(*registry, sendMessage(_, _)).Times(0);

  auto res = svc->sendMessage(R"({"senderUuid":"u1","receiverUuid":"u2","content":"hello"})");
  EXPECT_FALSE(res.has_value());
}

TEST(MessageServiceTest, SendMessage_Success_NotifiesReceiverAndReturnsSenderPayload) {
  MockMessageRepository* messageRepo;
  std::shared_ptr<MockMessageSessionRegistry> registry;
  auto svc = makeService(messageRepo, registry);

  EXPECT_CALL(*messageRepo, save(_)).WillOnce(Return(true));
  EXPECT_CALL(*registry, sendMessage("u2", _)).WillOnce(Return(true));

  auto res = svc->sendMessage(R"({"senderUuid":"u1","receiverUuid":"u2","content":"hello"})");
  ASSERT_TRUE(res.has_value());
  EXPECT_NE(res->find("message_sent"), std::string::npos);
}

TEST(MessageServiceTest, SendMessage_Success_ReturnsSenderPayloadEvenIfReceiverOffline) {
  MockMessageRepository* messageRepo;
  std::shared_ptr<MockMessageSessionRegistry> registry;
  auto svc = makeService(messageRepo, registry);

  EXPECT_CALL(*messageRepo, save(_)).WillOnce(Return(true));
  EXPECT_CALL(*registry, sendMessage("u2", _)).WillOnce(Return(false));

  auto res = svc->sendMessage(R"({"senderUuid":"u1","receiverUuid":"u2","content":"hello"})");
  ASSERT_TRUE(res.has_value());
  EXPECT_NE(res->find("message_sent"), std::string::npos);
}

// ---------------------------------------------------------------------------
// loadConversations
// ---------------------------------------------------------------------------

TEST(MessageServiceTest, LoadConversations_NoValue_ReturnsNullopt) {
  MockMessageRepository* messageRepo;
  std::shared_ptr<MockMessageSessionRegistry> registry;
  auto svc = makeService(messageRepo, registry);

  EXPECT_CALL(*messageRepo, findConversationsByUserUuid("u1")).WillOnce(Return(std::nullopt));

  auto res = svc->loadConversations("u1");
  EXPECT_FALSE(res.has_value());
}

TEST(MessageServiceTest, LoadConversations_EmptyList_ReturnsResponse) {
  MockMessageRepository* messageRepo;
  std::shared_ptr<MockMessageSessionRegistry> registry;
  auto svc = makeService(messageRepo, registry);

  EXPECT_CALL(*messageRepo, findConversationsByUserUuid("u1"))
      .WillOnce(Return(std::vector<Message>{}));

  auto res = svc->loadConversations("u1");
  ASSERT_TRUE(res.has_value());
  EXPECT_NE(res->find("conversations_loaded"), std::string::npos);
}

TEST(MessageServiceTest, LoadConversations_Success_ReturnsResponse) {
  MockMessageRepository* messageRepo;
  std::shared_ptr<MockMessageSessionRegistry> registry;
  auto svc = makeService(messageRepo, registry);

  Message m;
  EXPECT_CALL(*messageRepo, findConversationsByUserUuid("u1"))
      .WillOnce(Return(std::vector<Message>{m}));

  auto res = svc->loadConversations("u1");
  ASSERT_TRUE(res.has_value());
  EXPECT_NE(res->find("conversations_loaded"), std::string::npos);
}
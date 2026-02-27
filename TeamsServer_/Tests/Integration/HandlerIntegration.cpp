
// #include "../../src/Core/Models/User.h"
// #include "../../src/Handlers/Handler.h"
// #include "../../src/Utils/Config.h"
// #include "../Mocks/MockAuthService.h"
// #include "../includes.h"

// class TestAuthHandler : public AuthHandler {
// public:
//   TestAuthHandler(asio::thread_pool &pool, MockAuthService *mockService)
//       : AuthHandler(pool) {
//     authService_ = std::unique_ptr<MockAuthService>(mockService); // inject mock
//   }
// };

// // Test Fixture
// class HandlerIntegration : public ::testing::Test {
// protected:
//   asio::thread_pool pool_{2};
//   std::unique_ptr<Handler> handler_;

//   void SetUp() override { handler_ = std::make_unique<Handler>(); }

//   void TearDown() override { pool_.join(); }
// };

// // Helper pour attendre la réponse async
// std::string asyncResponse;
// std::mutex mtx;
// std::condition_variable cv;

// auto callback = [](std::string result) {
//   {
//     std::lock_guard<std::mutex> lock(mtx);
//     asyncResponse = result;
//   }
//   cv.notify_one();
// };

// TEST_F(HandlerIntegration, LoginFlow_Success) {
//   // Préparer le payload
//   std::string payload =
//       R"({"type":"login","email":"d@gmail.com","password":"d"})";

//   // Appeler le handler
//   handler_->handle_type(payload, callback);

//   // Attendre la réponse
//   std::unique_lock<std::mutex> lock(mtx);
//   cv.wait_for(lock, std::chrono::seconds(1));

//   // Vérifier que la réponse contient le type attendu
//   ASSERT_NE(asyncResponse.find("login_response"), std::string::npos);
// }

// TEST_F(HandlerIntegration, RegisterFlow_Success) {
//   std::string payload =
//       R"({"type":"register","email":"new@example.com","username":"newuser","password":"pass123"})";

//   handler_->handle_type(payload, callback);

//   std::unique_lock<std::mutex> lock(mtx);
//   cv.wait_for(lock, std::chrono::seconds(1));

//   ASSERT_NE(asyncResponse.find("register_response"), std::string::npos);
// }

// TEST_F(HandlerIntegration, ValidateTokenFlow_Invalid) {
//   std::string payload = R"({"type":"validate_token","token":"badtoken"})";

//   handler_->handle_type(payload, callback);

//   std::unique_lock<std::mutex> lock(mtx);
//   cv.wait_for(lock, std::chrono::seconds(1));

//   ASSERT_NE(asyncResponse.find("validate_token_response"), std::string::npos);
//   ASSERT_NE(asyncResponse.find("invalid token"), std::string::npos);
// }

// TEST_F(HandlerIntegration, UnknownType) {
//   std::string payload = R"({"type":"unknown"})";

//   // On capture stderr pour vérifier le log
//   testing::internal::CaptureStderr();
//   handler_->handle_type(payload, callback);
//   std::string output = testing::internal::GetCapturedStderr();

//   ASSERT_NE(output.find("Unknown message type"), std::string::npos);
// }
#include "../../src/Core/Models/User.h"
#include "../../src/Utils/ResponseFormater.h"
#include "../includes.h"

TEST(ResponseFormaterTest, JsonEscapeBasic) {
  std::string input = "Hello \"World\" \\ Test \n \t";
  std::string expected = "Hello \\\"World\\\" \\\\ Test \\n \\t";
  EXPECT_EQ(ResponseFormater::json_escape(input), expected);
}

TEST(ResponseFormaterTest, FormatUserResponseBasic) {
  User user;
  user.username = "Alice";
  user.email = "alice@example.com";
  user.uuid = "1234-5678";
  user.token = "abcd1234";
  user.status = "online";

  std::string result =
      ResponseFormater::format_user_response("login_response", user);

  std::string expected_start = "{\"type\":\"login_response\",\"data\":{";
  std::string expected_username = "\"username\":\"Alice\"";
  std::string expected_email = "\"email\":\"alice@example.com\"";
  std::string expected_uuid = "\"uuid\":\"1234-5678\"";
  std::string expected_token = "\"token\":\"abcd1234\"";
  std::string expected_status = "\"status\":\"online\"";

  // On vérifie que toutes les clés et valeurs attendues sont présentes
  EXPECT_NE(result.find(expected_start), std::string::npos);
  EXPECT_NE(result.find(expected_username), std::string::npos);
  EXPECT_NE(result.find(expected_email), std::string::npos);
  EXPECT_NE(result.find(expected_uuid), std::string::npos);
  EXPECT_NE(result.find(expected_token), std::string::npos);
  EXPECT_NE(result.find(expected_status), std::string::npos);
}

TEST(ResponseFormaterTest, FormatUserResponseWithEscapes) {
  User user;
  user.username = "A\"l\\i\nc\te";
  user.email = "em\"ail@example.com";
  user.uuid = "uuid\\123";
  user.token = "tok\nen";
  user.status = "on\"line";

  std::string result = ResponseFormater::format_user_response("resp", user);

  // Vérifie que les caractères spéciaux ont été échappés
  EXPECT_NE(result.find("\\\""), std::string::npos); // guillemets échappés
  EXPECT_NE(result.find("\\\\"), std::string::npos); // backslash échappé
  EXPECT_NE(result.find("\\n"), std::string::npos);  // newline échappé
  EXPECT_NE(result.find("\\t"), std::string::npos);  // tab échappé
}
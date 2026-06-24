#include "MessageService.h"
#include "../../Utils/PacketHelper.h"
#include "../../Utils/Crypto.h"
#include "../../Utils/ResponseFormater.h"



std::optional<std::string> MessageService::sendMessage(const std::string& payload) {
  auto messageJson = PacketHelper::extractValue(payload, "content");
  if (messageJson.empty()) {
    std::cout << "[MessageService] sendMessage: Missing 'content' field in payload" << std::endl;
    return std::nullopt;
  } 

  Message message = message_from_json(payload);
  // msg.id = Crypto::generate_uuid_v7(); // A ajouter plus tard avec un vrai système d'ajout local côté client. (UUID tmp coté client à remplacer par UUID généré côté serveur)

  if (!messageRepo_->save(message)) {
    return std::nullopt;
  }
  std::string payload_receiver = ResponseFormater::format_message_response("new_message", message);
  messageSessionRegistry_->sendMessage(message.receiver_id, payload_receiver);
  // Ici avec le return de sendMessage, on pourrait faire le "double check" de whatsapp pour savoir si le message a été distribué au client


  std::string payload_sender = ResponseFormater::format_message_response("message_sent", message);
  // Renvoie au client
  return payload_sender;
}

std::optional<std::string> MessageService::loadConversations(const std::string& userUuid) {
  std::optional<std::vector<Message>> conversations = messageRepo_->findConversationsByUserUuid(userUuid);

  if (!conversations.has_value()) {
    return std::nullopt;
  } return ResponseFormater::format_conversations_response("conversations_loaded", conversations.value());
}
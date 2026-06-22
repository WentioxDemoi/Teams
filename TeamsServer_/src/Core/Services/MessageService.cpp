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

  Message msg = message_from_json(payload);
  // msg.id = Crypto::generate_uuid_v7(); // A ajouter plus tard avec un vrai système j'ajout local côté client. (UUID tmp coté client à remplacer par UUID généré côté serveur)

  if (!messageRepo_->save(msg)) {
    return std::nullopt;
  }
  
  std::string payload_receiver = ResponseFormater::format_message_response("new_message", msg);

  // Envoie au destinataire
  messageSessionRegistry_->sendMessage(msg.receiver_id, payload_receiver);
  // Ici avec le return de sendMessage, on pourrait faire le "double check" de whatsapp pour savoir si le message a été distribué au client


  std::string payload_sender = ResponseFormater::format_message_response("message_sent", msg);
  // Renvoie au client
  return payload_sender;
}
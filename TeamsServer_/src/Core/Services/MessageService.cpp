#include "MessageService.h"
#include "../../Utils/PacketHelper.h"
#include "../../Utils/Crypto.h"


std::optional<std::string> MessageService::sendMessage(const std::string& payload) {
  auto messageJson = PacketHelper::extractValue(payload, "message");
  if (messageJson.empty()) return std::nullopt;

  Message msg = message_from_json(payload);
  msg.id = Crypto::generate_uuid_v7();

  if (!messageRepo_->save(msg)) {
    return std::nullopt;
  }

  std::string tmp = payload;

  // Envoie au destinataire
  PacketHelper::insertValue(tmp, "fromMe", "false");
  PacketHelper::insertValue(tmp, "id", msg.id);
  messageSessionRegistry_->sendMessage(msg.receiver_id, tmp);
  // Ici avec le return de sendMessage, on pourrait faire le "double check" de whatsapp pour savoir si le message a été distribué au client
  
  // Renvoie au client
  PacketHelper::insertValue(tmp, "fromMe", "true");
  return tmp;
}
#ifndef CALLSERVICE_H
#define CALLSERVICE_H

#include <QJsonObject>
#include <QObject>
#include "../Interfaces/ICallService.h"
#include "../Network/NetworkService.h"

/**
 * @class CallService
 * @brief Service de gestion des appels.
 *
 * Envoie et reçoit les événements d'appel via NetworkService sur le port 8083.
 */
class CallService : public ICallService {
  Q_OBJECT
 public:
  explicit CallService(NetworkService* network = nullptr, QObject* parent = nullptr);

 public slots:
  void startCall(const QString& calleeUuid) override;
  void acceptCall(const QString& callUuid) override;
  void hangup(const QString& callUuid) override;
  void disconnectFromServer() override;

 private slots:
  void handleServerResponse(const QJsonObject& root);

 private:
  NetworkService* network_;
};

#endif

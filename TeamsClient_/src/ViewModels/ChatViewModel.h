#ifndef CHATVIEWMODEL_H
#define CHATVIEWMODEL_H

#include <QObject>
#include <QString>
#include <QVariantMap>

#include "../Models/MessageList.h"
#include "../Models/UserList.h"
#include "Interfaces/IMessageService.h"
// #include "../Services/Interfaces/IChatService.h"

/**
 * @class ChatViewModel
 * @brief ViewModel pour l'écran principal de chat.
 *
 * Sert de couche intermédiaire entre l'interface utilisateur et les services de chat.
 * Gère la liste des contacts, les messages du contact sélectionné, et la sélection active.
 * Émet des signaux en cas d'erreur ou de changement d'état.
 */
class ChatViewModel : public QObject {
  Q_OBJECT
  // Inutile car déjà exposé dès le début à QML

  Q_PROPERTY(UserList* userList READ userList CONSTANT)

  // Exposer la liste de message afin que QML puisse être notifié des changements.
  Q_PROPERTY(MessageList* currentMessageList READ messageList NOTIFY messageListChanged)

  // Exposer le User actuel (sous forme de QVariant car QML ne connait et ne sais pas utiliser la class User)
  Q_PROPERTY(QVariantMap selectedUser READ selectedUser NOTIFY selectedUserChanged)

 public:
  explicit ChatViewModel(UserList* user = nullptr,
                         IMessageService* messageService = nullptr, // A implémenter plus tard
                         QObject* parent = nullptr);

  UserList* userList() const;
  MessageList* messageList() const;
  QVariantMap selectedUser() const;

 public slots:
  void selectUser(const QString& userUuid);
  void sendMessage(const QString& content);
  void loadContacts();

  //   void callUser(const QString& userUuid);

 signals:
  //   void callStarted(const QString& userUuid);
  //   void callError(const QString& error);

  void messageListChanged();
  void selectedUserChanged();
  void chatError(const QString& error);

 private:
  IMessageService* messageService_;
  UserList* userList_;
  QHash<QString, MessageList*> messagesByUuid_;
  MessageList* currentMessageList_;  // Displayed by QML
  QVariantMap selectedUser_;
};

#endif
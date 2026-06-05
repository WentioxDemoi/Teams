#ifndef CHATVIEWMODEL_H
#define CHATVIEWMODEL_H

#include <QObject>
#include <QString>

#include "../Models/User.h"
#include "../Models/UserList.h"
#include "../Models/MessageList.h"
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
  // Q_PROPERTY(UserList* userList READ userList CONSTANT)

  // A voir si on en a bien besoin normalement non car on va aussi l'exposer dès le début
  Q_PROPERTY(MessageList* messageList READ messageList NOTIFY messageListChanged)
  Q_PROPERTY(User* selectedUser READ selectedUser NOTIFY selectedUserChanged)

 public:
  explicit ChatViewModel(UserList *user = nullptr,
    //IChatService* service = nullptr, // A implémenter plus tard
                         QObject* parent = nullptr);

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

  UserList* userList() const;
  MessageList* messageList() const;
  User* selectedUser() const;

 private:
//   IChatService* chatService_;
  UserList* userList_;
  QHash<QString, MessageList*> messagesByUuid_;  // toutes les conversations
  MessageList* currentMessageList_;  
  User* selectedUser_;
};

#endif
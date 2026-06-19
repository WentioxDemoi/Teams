#ifndef CHATVIEWMODEL_H
#define CHATVIEWMODEL_H

#include <QObject>
#include <QString>
#include <QVariantMap>

#include "../Models/MessageList.h"
#include "../Models/SearchResults.h"
#include "../Models/UserList.h"
#include "../Models/User.h"
#include "Interfaces/IChatService.h"
#include "StateLocator.h"
#include "State/SessionState.h"

class MessageRepository; // TMP

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
  Q_PROPERTY(SearchResults* searchResults READ searchResults CONSTANT)

 public:
  explicit ChatViewModel(UserList* user = nullptr,
                         IChatService* chatService = nullptr,
                         SessionState* sessionState = nullptr,
                         SearchResults* searchResults = nullptr,
                         QObject* parent = nullptr);

  UserList* userList() const;
  MessageList* messageList() const;
  QVariantMap selectedUser() const;
  SearchResults* searchResults() const { return searchResults_; }

 public slots:
  void selectUser(const QString& userUuid);
  void searchUsers(const QString& query); // Used by QML
  void sendMessage(const QString& content);
  void onContactsSearchLoaded(const QList<User> users);

  //   void callUser(const QString& userUuid);

 private slots:
  void onLocalUserSaved(const User& user); // TMP
  void onContactsLoaded(const QList<User>& users);
  void onMessagesLoaded(const QList<Message>& messages);
  void onApplicationQuit();
 signals:
  //   void callStarted(const QString& userUuid);
  //   void callError(const QString& error);

  void messageListChanged();
  void selectedUserChanged();
  void chatError(const QString& error);

 private:
  // void refreshConversationsFromDatabase();
  void persistMessage(const Message& message);
  void seedDatabaseMessages(const QString& localUuid);

  IChatService* chatService_;
  UserList* userList_;
  MessageRepository* messageRepository_; // TMP
  QHash<QString, MessageList*> messagesByUuid_;
  MessageList* currentMessageList_;  // Displayed by QML
  QVariantMap selectedUser_;
  SessionState* sessionState_;
  SearchResults* searchResults_;
};

#endif
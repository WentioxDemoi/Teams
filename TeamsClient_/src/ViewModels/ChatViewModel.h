#ifndef CHATVIEWMODEL_H
#define CHATVIEWMODEL_H

#include <QObject>
#include <QString>
#include <QVariantMap>

#include "../Models/ContactList.h"
#include "../Models/MessageList.h"
#include "../Models/SearchResults.h"
#include "../Models/User.h"
#include "Interfaces/IChatService.h"
#include "Interfaces/IContactService.h"
#include "State/SessionState.h"

class MessageRepository; // TMP

/**
 * @class ChatViewModel
 * @brief ViewModel pour l'écran principal de chat.
 *
 * Sert de couche intermédiaire entre l'interface utilisateur et les services de
 * chat. Gère la liste des contacts, les messages du contact sélectionné, et la
 * sélection active. Émet des signaux en cas d'erreur ou de changement d'état.
 */
class ChatViewModel : public QObject {
  Q_OBJECT
  // Inutile car déjà exposé dès le début à QML

  Q_PROPERTY(ContactList *contactList READ contactList CONSTANT)

  // Exposer la liste de message afin que QML puisse être notifié des
  // changements.
  Q_PROPERTY(MessageList *currentMessageList READ messageList NOTIFY
                 messageListChanged)

  // Exposer le User actuel (sous forme de QVariant car QML ne connait et ne
  // sais pas utiliser la class User)
  Q_PROPERTY(QVariantMap selectedContact READ selectedContact NOTIFY
                 selectedContactChanged)
  Q_PROPERTY(SearchResults *searchResults READ searchResults CONSTANT)

public:
  explicit ChatViewModel(ContactList *user = nullptr,
                         IChatService *chatService = nullptr,
                         IContactService *contactService = nullptr,
                         SessionState *sessionState = nullptr,
                         SearchResults *searchResults = nullptr,
                         QObject *parent = nullptr);

  ContactList *contactList() const;
  MessageList *messageList() const;
  QVariantMap selectedContact() const;
  SearchResults *searchResults() const { return searchResults_; }

  void selectUser(const QString &userUuid);
  void selectContact(const QString &contactUuid);
  void searchUsers(const QString &query); // Used by QML
  void sendMessage(const QString &content);

signals:
  void messageListChanged();
  void selectedContactChanged();
  void chatError(const QString &error);

private:
  void onContactsLoaded(const QList<User> &contacts);
  void onMessagesLoaded(const QList<Message> &messages);
  void onApplicationQuit();
  
  void activateConversation(const QString &userUuid);

  IChatService *chatService_;
  IContactService *contactService_;
  ContactList *contactList_;
  // MessageRepository* messageRepository_; // TMP
  QHash<QString, MessageList *> messagesByUuid_;
  MessageList *currentMessageList_; // Displayed by QML
  QVariantMap selectedContact_;
  SessionState *sessionState_;
  SearchResults *searchResults_;
};

#endif
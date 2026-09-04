#ifndef IMESSAGEREPOSITORY_H
#define IMESSAGEREPOSITORY_H

#include "Models/Message.h"

#include <QList>
#include <QObject>

class IMessageRepository : public QObject {
  Q_OBJECT

 public:
  explicit IMessageRepository(QObject* parent = nullptr) : QObject(parent) {}
  ~IMessageRepository() override = default;

  virtual QList<Message> findAll() = 0;
  virtual bool save(const Message& message) = 0;
  virtual bool removeAll() = 0;
};

#endif
#ifndef SESSIONSTATE_H
#define SESSIONSTATE_H

#include "User.h"
#include <QObject>

class SessionState : public QObject {
    Q_OBJECT

public:
    const User& localUser() const { return user_; }

public slots:
    void setlocalUser(const User& user) {
        if (user_.uuid() == user.uuid())
            return;

        user_ = user;
        emit localUserChanged(user_);
    }

signals:
    void localUserChanged(const User &user_);

private:
    User user_;
};

#endif
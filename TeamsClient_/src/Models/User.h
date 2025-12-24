#ifndef USER_H
#define USER_H

#include <QString>
#include <QJsonObject>

class User {
public:
    User() = default;
    User(int id,
         const QString& username,
         const QString& token,
         const QString& status);

    // Getters
    int id() const { return id_; }
    QString username() const { return username_; }
    QString token() const { return token_; }
    QString status() const { return status_; }

    // Setters
    void setId(int id) { id_ = id; }
    void setUsername(const QString& username) { username_ = username; }
    void setToken(const QString& token) { token_ = token; }
    void setStatus(const QString& status) { status_ = status; }

    // Serialization
    QJsonObject toJson() const;
    static User fromJson(const QJsonObject& json);

    bool isValid() const { return id_ > 0 && !username_.isEmpty(); }

private:
    int id_ = 0;
    QString username_;
    QString token_;
    QString status_;
};

#endif // USER_H
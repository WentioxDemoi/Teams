// #ifndef MESSAGEMANAGER_H
// #define MESSAGEMANAGER_H

// #include <QObject>
// #include <QString>
// #include <boost/asio.hpp>
// #include <atomic>

// class MessageManager : public QObject {
//     Q_OBJECT

// public:
//     explicit MessageManager(boost::asio::io_context& io, const std::string& host, unsigned short port, QObject* parent = nullptr);
//     ~MessageManager();

// signals:
//     void messageReceived(const QString& msg);
//     void errorOccurred(const QString& err);

// public slots:
//     void connectToServer();
//     void sendMessage(const QString& message);
//     void disconnect();

// private:
//     void readMessages();

//     boost::asio::ip::tcp::socket socket_;
//     boost::asio::ip::tcp::resolver resolver_;
//     std::string host_;
//     unsigned short port_;
//     std::atomic<bool> connected_;
// };

// #endif
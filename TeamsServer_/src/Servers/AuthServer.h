#ifndef AUTHSERVER_H
#define AUTHSERVER_H

#include "../Errors/ErrorManager.h"
#include "../Sessions/AuthSession.h"
#include "../includes.h"

class AuthServer : public ErrorManager {
public:
  AuthServer(asio::io_context &io_context, ssl::context &ssl_ctx,
             tcp::endpoint endpoint, asio::thread_pool &db_pool);

private:
  void do_accept();

  tcp::acceptor acceptor_;
  ssl::context &ssl_ctx_;
  asio::thread_pool &db_pool_;
};

#endif
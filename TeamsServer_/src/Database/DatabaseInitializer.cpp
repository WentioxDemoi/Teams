#include "DatabaseInitializer.h"

void DatabaseInitializer::init() {
  auto &ctx = DatabaseContext::instance();

  auto conn = ctx.acquire();
  pqxx::work txn(*conn);

  txn.exec("CREATE TABLE IF NOT EXISTS " + ctx.users_table() +
         " (id SERIAL PRIMARY KEY, "
         "server_id TEXT UNIQUE NOT NULL,"
         "email TEXT UNIQUE NOT NULL, "
         "username TEXT UNIQUE, "
         "password_hash TEXT NOT NULL, "
         "token TEXT, "
         "token_expires_at TIMESTAMP, "
         "status TEXT DEFAULT 'offline', "
         "last_seen TIMESTAMP DEFAULT NOW(), "
         "created_at TIMESTAMP DEFAULT NOW());");

  txn.exec(
      "CREATE TABLE IF NOT EXISTS " + ctx.channels_table() +
      " (id SERIAL PRIMARY KEY, name TEXT NOT NULL, "
      "is_private BOOLEAN DEFAULT TRUE, created_at TIMESTAMP DEFAULT NOW());");

  txn.exec("CREATE TABLE IF NOT EXISTS " + ctx.channel_members_table() +
           " (channel_id INT REFERENCES " + ctx.channels_table() +
           "(id), user_id INT REFERENCES " + ctx.users_table() +
           "(id), PRIMARY KEY(channel_id, user_id));");

  txn.exec("CREATE TABLE IF NOT EXISTS " + ctx.messages_table() +
           " (id SERIAL PRIMARY KEY, channel_id INT REFERENCES " +
           ctx.channels_table() + "(id), sender_id INT REFERENCES " +
           ctx.users_table() +
           "(id), content TEXT NOT NULL, created_at TIMESTAMP DEFAULT NOW());");

  txn.commit();
  ctx.release(conn);
}
// infrastructure/database/QueryBuilder.h
#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include "../../includes.h"

/**
 * @class QueryBuilder
 * @brief Constructeur de requêtes SQL de manière programmatique et fluide.
 *
 * Permet de créer des requêtes SELECT, INSERT, UPDATE et DELETE en chaînant
 * des appels de méthode, facilitant la lecture et la maintenance du code.
 */
class QueryBuilder {
public:
  QueryBuilder() = default;

  // SELECT
  QueryBuilder &select(const std::vector<std::string> &columns = {"*"});
  QueryBuilder &from(const std::string &table);

  // WHERE
  QueryBuilder &where(const std::string &column, const std::string &op,
                      const std::string &placeholder);
  QueryBuilder &and_where(const std::string &column, const std::string &op,
                          const std::string &placeholder);
  QueryBuilder &or_where(const std::string &column, const std::string &op,
                         const std::string &placeholder);

  // JOIN
  QueryBuilder &join(const std::string &table, const std::string &condition);
  QueryBuilder &left_join(const std::string &table,
                          const std::string &condition);

  // ORDER BY
  QueryBuilder &order_by(const std::string &column,
                         const std::string &direction = "ASC");

  // LIMIT / OFFSET
  QueryBuilder &limit(int count);
  QueryBuilder &offset(int count);

  // INSERT
  QueryBuilder &insert_into(const std::string &table,
                            const std::vector<std::string> &columns = {});
  QueryBuilder &values(const std::vector<std::string> &placeholders);
  QueryBuilder &returning(const std::vector<std::string> &columns);

  // UPDATE
  QueryBuilder &update(const std::string &table);
  QueryBuilder &set(const std::string &column, const std::string &placeholder);

  // DELETE
  QueryBuilder &delete_from(const std::string &table);

  // BUILD
  std::string build() const;

  // RESET
  void reset();

private:
  enum class QueryType { SELECT, INSERT, UPDATE, DELETE };

  QueryType type_;
  std::string table_;
  std::vector<std::string> select_columns_;
  std::vector<std::string> where_clauses_;
  std::vector<std::string> join_clauses_;
  std::vector<std::string> order_clauses_;
  std::vector<std::string> set_clauses_;
  std::vector<std::string> insert_values_;
  std::vector<std::string> returning_columns_;
  std::vector<std::string> insert_columns_;
  std::optional<int> limit_;
  std::optional<int> offset_;

  void append_where(const std::string &clause, const std::string &conjunction);
};

#endif
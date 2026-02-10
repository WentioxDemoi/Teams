#include "QueryBuilder.h"

QueryBuilder &QueryBuilder::select(const std::vector<std::string> &columns) {
  type_ = QueryType::SELECT;
  select_columns_ = columns;
  return *this;
}

QueryBuilder &QueryBuilder::from(const std::string &table) {
  table_ = table;
  return *this;
}

QueryBuilder &QueryBuilder::where(const std::string &column,
                                  const std::string &op,
                                  const std::string &placeholder) {
  append_where(column + " " + op + " " + placeholder, "WHERE");
  return *this;
}

QueryBuilder &QueryBuilder::and_where(const std::string &column,
                                      const std::string &op,
                                      const std::string &placeholder) {
  append_where(column + " " + op + " " + placeholder, "AND");
  return *this;
}

QueryBuilder &QueryBuilder::or_where(const std::string &column,
                                     const std::string &op,
                                     const std::string &placeholder) {
  append_where(column + " " + op + " " + placeholder, "OR");
  return *this;
}

void QueryBuilder::append_where(const std::string &clause,
                                const std::string &conjunction) {
  if (where_clauses_.empty()) {
    where_clauses_.push_back("WHERE " + clause);
  } else {
    where_clauses_.push_back(conjunction + " " + clause);
  }
}

QueryBuilder &QueryBuilder::join(const std::string &table,
                                 const std::string &condition) {
  join_clauses_.push_back("JOIN " + table + " ON " + condition);
  return *this;
}

QueryBuilder &QueryBuilder::left_join(const std::string &table,
                                      const std::string &condition) {
  join_clauses_.push_back("LEFT JOIN " + table + " ON " + condition);
  return *this;
}

QueryBuilder &QueryBuilder::order_by(const std::string &column,
                                     const std::string &direction) {
  order_clauses_.push_back(column + " " + direction);
  return *this;
}

QueryBuilder &QueryBuilder::limit(int count) {
  limit_ = count;
  return *this;
}

QueryBuilder &QueryBuilder::offset(int count) {
  offset_ = count;
  return *this;
}

QueryBuilder &
QueryBuilder::insert_into(const std::string &table,
                          const std::vector<std::string> &columns) {
  type_ = QueryType::INSERT;
  table_ = table;
  insert_columns_ = columns;
  return *this;
}

QueryBuilder &
QueryBuilder::values(const std::vector<std::string> &placeholders) {
  insert_values_ = placeholders;
  return *this;
}

QueryBuilder &QueryBuilder::returning(const std::vector<std::string> &columns) {
  returning_columns_ = columns;
  return *this;
}

QueryBuilder &QueryBuilder::update(const std::string &table) {
  type_ = QueryType::UPDATE;
  table_ = table;
  return *this;
}

QueryBuilder &QueryBuilder::set(const std::string &column,
                                const std::string &placeholder) {
  set_clauses_.push_back(column + " = " + placeholder);
  return *this;
}

QueryBuilder &QueryBuilder::delete_from(const std::string &table) {
  type_ = QueryType::DELETE;
  table_ = table;
  return *this;
}

std::string QueryBuilder::build() const {
  std::ostringstream query;

  switch (type_) {
  case QueryType::SELECT: {
    query << "SELECT ";
    for (size_t i = 0; i < select_columns_.size(); ++i) {
      query << select_columns_[i];
      if (i < select_columns_.size() - 1)
        query << ", ";
    }
    query << " FROM " << table_;

    for (const auto &join : join_clauses_) {
      query << " " << join;
    }

    for (const auto &where : where_clauses_) {
      query << " " << where;
    }

    if (!order_clauses_.empty()) {
      query << " ORDER BY ";
      for (size_t i = 0; i < order_clauses_.size(); ++i) {
        query << order_clauses_[i];
        if (i < order_clauses_.size() - 1)
          query << ", ";
      }
    }

    if (limit_)
      query << " LIMIT " << *limit_;
    if (offset_)
      query << " OFFSET " << *offset_;
    break;
  }

  case QueryType::INSERT: {
    query << "INSERT INTO " << table_;

    // AJOUTER CE BLOC
    if (!insert_columns_.empty()) {
      query << " (";
      for (size_t i = 0; i < insert_columns_.size(); ++i) {
        query << insert_columns_[i];
        if (i < insert_columns_.size() - 1)
          query << ", ";
      }
      query << ")";
    }

    query << " VALUES (";
    for (size_t i = 0; i < insert_values_.size(); ++i) {
      query << insert_values_[i];
      if (i < insert_values_.size() - 1)
        query << ", ";
    }
    query << ")";

    if (!returning_columns_.empty()) {
      query << " RETURNING ";
      for (size_t i = 0; i < returning_columns_.size(); ++i) {
        query << returning_columns_[i];
        if (i < returning_columns_.size() - 1)
          query << ", ";
      }
    }
    break;
  }

  case QueryType::UPDATE: {
    query << "UPDATE " << table_ << " SET ";
    for (size_t i = 0; i < set_clauses_.size(); ++i) {
      query << set_clauses_[i];
      if (i < set_clauses_.size() - 1)
        query << ", ";
    }

    for (const auto &where : where_clauses_) {
      query << " " << where;
    }

    if (!returning_columns_.empty()) {
      query << " RETURNING ";
      for (size_t i = 0; i < returning_columns_.size(); ++i) {
        query << returning_columns_[i];
        if (i < returning_columns_.size() - 1)
          query << ", ";
      }
    }
    break;
  }

  case QueryType::DELETE: {
    query << "DELETE FROM " << table_;

    for (const auto &where : where_clauses_) {
      query << " " << where;
    }

    if (!returning_columns_.empty()) {
      query << " RETURNING ";
      for (size_t i = 0; i < returning_columns_.size(); ++i) {
        query << returning_columns_[i];
        if (i < returning_columns_.size() - 1)
          query << ", ";
      }
    }
    break;
  }
  }

  return query.str();
}

void QueryBuilder::reset() {
  table_.clear();
  select_columns_.clear();
  where_clauses_.clear();
  join_clauses_.clear();
  order_clauses_.clear();
  set_clauses_.clear();
  insert_values_.clear();
  returning_columns_.clear();
  limit_.reset();
  offset_.reset();
}
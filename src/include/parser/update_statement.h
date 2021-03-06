//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// statement_update.h
//
// Identification: src/include/parser/statement_update.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "expression/abstract_expression.h"
#include "optimizer/query_node_visitor.h"
#include "parser/sql_statement.h"
#include "parser/table_ref.h"

namespace peloton {
namespace parser {

/**
 * @struct UpdateClause
 * @brief Represents "column = value" expressions
 */
class UpdateClause {
 public:
  char* column;
  expression::AbstractExpression* value;

  ~UpdateClause() {
    if (column != nullptr) {
      delete column;
    }
    if (value != nullptr) {
      delete value;
    }
  }

  UpdateClause* Copy() {
    UpdateClause* new_clause = new UpdateClause();
    std::string str(column);
    char* new_cstr = new char[str.length() + 1];
    std::strcpy(new_cstr, str.c_str());
    expression::AbstractExpression* new_expr = value->Copy();
    new_clause->column = new_cstr;
    new_clause->value = new_expr;
    return new_clause;
  }
};

/**
 * @struct UpdateStatement
 * @brief Represents "UPDATE"
 */
struct UpdateStatement : SQLStatement {
  UpdateStatement()
      : SQLStatement(STATEMENT_TYPE_UPDATE),
        table(NULL),
        updates(NULL),
        where(NULL) {}

  virtual ~UpdateStatement() {
    delete table;

    if (updates) {
      for (auto clause : *updates) delete clause;
    }

    delete updates;
    if (where != NULL) delete where;
  }

  virtual void Accept(optimizer::QueryNodeVisitor* v) const override {
    v->Visit(this);
  }

  // TODO: switch to char* instead of TableRef
  TableRef* table;
  std::vector<UpdateClause*>* updates;
  expression::AbstractExpression* where = nullptr;
};

}  // End parser namespace
}  // End peloton namespace

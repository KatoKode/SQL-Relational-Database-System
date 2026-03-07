//-----------------------------------------------------------------------------
// Desc: SQL library
//
// Copyright © 2019 J McIntosh
//
//   This file is part of redB.
//
//   redB is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; version 3 of the License.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with redB.  If not, see <https://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------

#ifndef SQL_H
#define SQL_H 1

#include <utility>
#include <string>
#include "err.h"
#include "tran.h"
#include "comp.h"

#undef SQL_DEBUG

/*
 +-----------------------------------------------------------------------------
 | SQL Statement Syntax
 +-----------------------------------------------------------------------------
 |  stmt_list: stmt ';'
 |    | stmt_list stmt ';'
 |    ;
 |
 |  stmt: create_stmt
 |    | delete_stmt
 |    | drop_stmt
 |    | insert_stmt
 |    | select_stmt
 |    | use_stmt
 |    ;
 |
 |  create_stmt: create_database
 |    | create_table
 |    ;
 |
 |  create_database: CREATE DATABASE NAME
 |    ;
 |
 |  create_table: CREATE TABLE NAME '(' column_definition_list ')'
 |    ;
 |
 |  column_definition_list: column_definition
 |    | column_definition_list ',' column_definition
 |    ;
 |
 |  column_definition: NAME data_type
 |    | PRIMARY KEY '(' NAME ')'
 |    ;
 |
 |  data_type: CHAR '(' INT ')'
 |    | INT
 |    | FLOAT '(' INT ',' INT ')'
 |    | VARCHAR '(' INT ')'
 |    ;
 |
 |  delete_stmt: TODO
 |    ;
 |
 |  drop_stmt: drop_database
 |    | drop_table
 |    ;
 |
 |  drop_database: DROP DATABASE NAME
 |    ;
 |
 |  drop_table: DROP TABLE NAME
 |    ;
 |
 |  insert_stmt: INSERT INTO NAME opt_column_list
 |    VALUES insert_value_list
 |    ;
 |
 |  opt_column_list:
 |    | '(' column_list ')'
 |    ;
 |
 |  column_list: NAME
 |    | column_list ',' NAME
 |    ;
 |
 |  insert_value_list: '(' value_list ')'
 |    ;
 |
 |  value_list: expr
 |    | value_list ',' expr
 |    ;
 |
 |  select_stmt: SELECT select_expr_list FROM table_reference_list opt_where
 |    ;
 |
 |  select_expr_list: select_expr
 |    | select_expr_list ',' select_expr
 |    ;
 |
 |  select_expr: expr
 |    ;
 |
 |  table_reference_list: table_reference
 |    | table_reference_list ',' table_reference
 |    ;
 |
 |  table_reference: NAME opt_as_alias
 |    | NAME '.' NAME opt_as_alias
 |    ;
 |
 |  opt_as_alias: AS NAME
 |    |
 |    ;
 |
 |  opt_where: WHERE expr
 |    ;
 |
 |  expr: FLOAT
 |    | INT
 |    | NAME
 |    | NAME '.' NAME
 |    | NAME '.' '*'
 |    ;
 |
 |  expr: expr '*' expr
 |    | expr '+' expr
 |    | expr '-' expr
 |    | expr '/' expr
 |    ;
 |
 |  expr: expr '<' expr
 |    | expr '>' expr
 |    | expr '==' expr
 |    | expr '<=' expr
 |    | expr '>=' expr
 |    ;
 |
 |  use_stmt: USE NAME
 |    ;
 |
 +-----------------------------------------------------------------------------
 */

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS SQL
   */
  class sql {
    public:
      sql (std::string const& stmt) : m_stmt {stmt} { }

      sql (sql const&) = delete;

      sql& operator=(sql const&) = delete;

      std::pair<red::buffer, red::buffer> compile_stmt ();
    private:
      std::string     m_stmt;
  };

} // namespace

#endif  // SQL_H


//-----------------------------------------------------------------------------
// Desc: SQL lexer/parser library (translator)
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

#ifndef TRAN_H
#define TRAN_H 1

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>
#include "eno.h"
#include "logger.h"
#include "schema.h"
#include "token.h"

#undef TRAN_DEBUG

namespace red::tran {

  class parser;

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS LEXER
   */
  class lexer {
    public:
      static char const EOI = static_cast<char>(255);

      lexer (std::string const& input)
        : m_input {input}, m_mark {0}, m_index {0}, m_ch {'\0'}
      {
        init();
      }

      lexer (lexer const&) = delete;

      lexer (lexer&& o)
        : m_input {o.m_input}, m_mark {o.m_mark}, m_index {o.m_index},
        m_ch {o.m_ch}, m_token_list {o.m_token_list}
      { }

      lexer& operator=(lexer const&) = delete;
      lexer& operator=(lexer&&);

      std::string remaining () const;
      token next_token ();
    private:
      std::string m_input;
      size_t      m_mark;
      size_t      m_index;
      char        m_ch;
      std::vector<token>  m_token_list;

      void comment ();
      void consume ();
      token compound ();
      int32_t get_symbol_type (std::string const&);
      void init ();
      token name ();
      token number ();
      char peek ();
      token varchar ();
      void ws ();

      friend class parser;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS PARSER
   */
  class parser {
    public:
      parser (lexer& lxr)
        : m_lexer {std::move(lxr)},
        m_index {0L},
        m_mark{-1L},
        m_use_asterisk {true}
      {
        init();
      }

      parser (parser const&) = delete;

      parser& operator=(parser const&) = delete;

      std::string dump () const;
      std::string dump_data (size_t i) const { return m_data.at(i).text(); }
      std::string dump_text (size_t i) const { return m_code.at(i); }
      void parse ();
    private:
      lexer                     m_lexer;
      std::vector<token>        m_lookahead;
      std::vector<std::string>  m_code;
      std::vector<token>        m_data;
      size_t                    m_index;
      ssize_t                   m_mark;
      bool                      m_use_asterisk;

      void init ();

      void consume ();
      std::string data (int32_t);
      std::string data (std::string const&);
      std::string find_data (std::string const&);
      token get_lookahead_token(size_t);
      int32_t get_lookahead_type(size_t i) {
        return get_lookahead_token(i).type();
      }
      void mark() { m_mark = m_index; }
      void match (int32_t);
      void match (std::vector<int32_t>&);
      bool is_speculating () const { return not (m_mark < 0); }
      void load_lookahead (size_t);
      void unmark() { m_index = m_mark; m_mark = -1L; }

      std::string next_header ();

      void stmt ();

      void alter_stmt ();
      void alter_database ();
      void alter_table ();
      void alter_user ();
      void alter_add (size_t const, std::vector<std::string>&);
      void add_column (size_t const);
      void add_foreign_key (size_t const, std::vector<std::string>&);
      void add_index (size_t const);
      void add_unique_index (size_t const);
      void index_key ();
      void alter_drop (size_t const);
      void drop_column (size_t const);
      void drop_foreign_key (size_t const);
      void drop_index (size_t const);

      void create_stmt ();
      void create_database ();
      void create_user ();
      void create_table ();
      void column_definition_list (std::vector<std::string>&);
      void column_definition (std::vector<std::string>&);
      void data_type (std::vector<std::string>&);
      void constraint (std::vector<std::string>&);
      void key_definition ();
      void foreign_key ();
      void primary_key ();
      void unique_key ();

      void delete_stmt ();

      void describe_stmt ();

      void drop_stmt ();
      void drop_database ();
      void drop_table ();
      void drop_user ();

      void grant_stmt ();

      void insert_stmt ();
      void opt_column_list();
      void column_list (int32_t&);
      void value_set_list ();
      void value_set ();
      void value_expr ();

      void load_stmt ();

      void revoke_stmt ();

      void select_stmt ();
      void select_expr_list (std::vector<std::string>&);
      void select_expr (std::vector<std::string>&);
      void table_reference_list (std::vector<std::string>&);
      void table_reference (std::vector<std::string>&);
      void opt_as_alias (std::vector<std::string>&);
      void join (std::vector<std::string>&);
      void on_using (std::vector<std::string>&);
      void opt_where ();
      void opt_order_by ();
      void opt_into_outfile ();

      void show_stmt ();

      void update_stmt ();
      void assignment_list ();
      void assignment ();

      void use_stmt ();

      void expr (std::vector<std::string>& code) { logic_expr(code); }
      void name (std::vector<std::string>&);

      void logic_expr (std::vector<std::string>&);
      void logic_primary (std::vector<std::string>&, bool);
      void logic_term (std::vector<std::string>&, bool);

      void comparison_expr (std::vector<std::string>&, bool);
      void comparison_primary (std::vector<std::string>&, bool);
      void comparison_term (std::vector<std::string>&, bool);

      void math_expr (std::vector<std::string>&, bool);
      void math_primary (std::vector<std::string>&, bool);
      void math_term (std::vector<std::string>&, bool);

      bool speculate_list (std::vector<int32_t>&);
      bool speculate_list_to (std::vector<int32_t>&);
  };

} // namespace

#endif  // TRAN_H

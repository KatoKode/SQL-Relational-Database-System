//-----------------------------------------------------------------------------
// Desc: SQL lexer/parser library (compiler)
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

#ifndef COMP_H
#define COMP_H 1

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "eno.h"
#include "logger.h"
#include "buffer.h"
#include "token.h"
#include "opcode.h"

#undef COMP_DEBUG

namespace red::comp {

  class comp_parser;

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS LEXER2
   */
  class comp_lexer {
    public:
      static char const EOI = static_cast<char>(255);

      comp_lexer (std::string const& input)
        : m_input {input}, m_mark {0}, m_index {0}, m_ch {'\0'}
      {
        init();
      }

      comp_lexer (comp_lexer const&) = delete;

      comp_lexer(comp_lexer&& o)
        : m_input {o.m_input},
        m_mark {o.m_mark},
        m_index {o.m_index},
        m_ch {o.m_ch}
      { }

      comp_lexer& operator=(comp_lexer const&) = delete;
      comp_lexer& operator=(comp_lexer&&);

      std::string remaining () const;
      token next_token ();
    private:
      std::string m_input;
      size_t  m_mark;
      size_t  m_index;
      char    m_ch;

      void consume ();
      token compound ();
      int32_t get_symbol_type (std::string const&);
      void init ();
      token name ();
      token number ();
      char peek ();
      token varchar ();
      void ws ();

      friend class comp_parser;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS PARSER2
   */
  class comp_parser {
      int32_t const CODE_CAPACITY = 8192;
      int32_t const DATA_CAPACITY = 8192;
    public:
      comp_parser (comp_lexer& lxr)
        : m_lexer {std::move(lxr)}, m_code {CODE_CAPACITY},
        m_data {DATA_CAPACITY}, m_index {0}
      {
        init();
      }

      comp_parser (comp_parser const&) = delete;

      comp_parser& operator=(comp_parser const&) = delete;

      void parse ();
#ifdef COMP_DEBUG
      void dump2 ();
#endif
      std::pair<red::buffer, red::buffer> dump ();
    private:
      comp_lexer  m_lexer;
      std::vector<token>  m_lookahead;
      buffer  m_code;
      buffer  m_data;
      size_t  m_index;

      void init ();

      void consume ();
      token get_lookahead_token(size_t);
      int32_t get_lookahead_type(size_t i) {
        return get_lookahead_token(i).type();
      }
      void load_lookahead (size_t);
      void match (int32_t);

      void call ();
      void compile ();
      void data (std::vector<int32_t>&);
      void data_segment (std::vector<int32_t>&);
      void push ();
      void pusho (std::vector<int32_t>&);
      void text (std::vector<int32_t>&);
      void text_segment (std::vector<int32_t>&);
  };

} // namespace

#endif  // COMP_H


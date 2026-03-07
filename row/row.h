//-----------------------------------------------------------------------------
// Desc: Row library
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

#ifndef ROW_H
#define ROW_H 1

#include <time.h>
#include <cstdint>
#include <cassert>
#include <ctime>
#include <list>
#include <string>
#include <cctype>
#include <algorithm>
#include <stdexcept>
#include "eno.h"
#include "logger.h"
#include "schema.h"
#include "buffer.h"
#include "../btree/const.h"

namespace red {

  /****************************************************************************
   * CLASS ROW
   */
  class row {
    public:
      row () : m_buffer {32} { }

      row (int32_t const size, std::shared_ptr<schema::schema> sch)
        : m_buffer {size}, m_schema {sch}
      { init(); }

      row (std::shared_ptr<schema::schema>, buffer&);

      ~row () { }

      row (row const& o) : m_buffer {o.m_buffer},
          m_schema {o.m_schema}
      { }

      row (row&& o) : m_buffer {o.m_buffer},
          m_schema {o.m_schema}
      { }

      row& operator=(row const&); 
      row& operator=(row&&);

      void append (size_t i, std::string const& value) {
        append(i, const_cast<std::string&>(value));
      }

      void append (size_t, std::string&);

      buffer backing ();

      void clear () { init(); }

      size_t column_pos (std::string const&);

      void copy_row (buffer&);

      std::string db_name () const {
        return m_schema->get_header().db_name();
      }

      std::shared_ptr<red::schema::schema> get_schema () { return m_schema; }

      void init (red::row&);

      std::string make_foreign_key (std::string const&);  // by name

      std::string make_key (std::string const&);  // by name

      void rewind () { m_buffer.rewind(); }

      int32_t size () {
        return static_cast<int32_t>(m_buffer.get_int16(tree::ROW_SIZE_POS));
      }

      void size (int32_t value) {
        m_buffer.put_int16(tree::ROW_SIZE_POS, static_cast<int16_t>(value));
      }

      std::string tbl_name () const {
        return m_schema->get_header().tbl_name();
      }

      bool verify_value (size_t const, std::string const&);

      std::string value_at (int32_t const);
      std::string value_at (std::string const&);
    private:
      buffer  m_buffer;
      std::shared_ptr<schema::schema>   m_schema;

      void init ();

      bool is_boolean_valid (std::string const& value) {
        return (value == "0" || value == "1");
      }

      bool is_date_valid (std::string const&);
      bool is_datetime_valid (std::string const&);
      bool is_enum_valid (size_t, std::string const&);

      bool is_float_valid (std::string const& value) {
        try { std::stof(value); } catch (...) { return false; }
        return true;
      }

      bool is_integer_valid (std::string const& value) {
        try { std::stoi(value); } catch (...) { return false; }
        return true;
      }

      bool is_timestamp_valid (std::string const&);
      bool not_null_compliant (size_t, std::string const&);
      bool is_valid (size_t, std::string const&);
  };

}   // namespace

#endif  // ROW_H


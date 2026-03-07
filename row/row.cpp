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

#include "row.h"

namespace red {

  /*
   * All row values are stored as std::string.
   */

  /****************************************************************************
   *
   */
  std::string to_lower (std::string const& value) {
    std::string temp = value;
    for (auto iter = temp.begin(); iter != temp.end(); ++iter)
      *iter = std::tolower(*iter);
    return temp;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS ROW: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS ROW: Constructor
   */
  row::row (std::shared_ptr<schema::schema> sch, buffer& buf)
    : m_buffer {static_cast<int32_t>(buf.get_int16(buf.index()
          + tree::ROW_SIZE_POS))},
    m_schema {sch}
  {
    buf.index(buf.index() - tree::ROW_SIZE_SZ);
    copy_row(buf);
  }

  /****************************************************************************
   * CALL ROW: Copy Assignment
   */
  row& row::operator=(row const& o) {
    if (this == &o) return *this;
    m_buffer  = o.m_buffer;
    m_schema  = o.m_schema;
    return *this;
  }

  /****************************************************************************
   * CALL ROW: Move Assignment
   */
  row& row::operator=(row&& o) {
    if (this == &o) return *this;
    m_buffer  = o.m_buffer;
    m_schema  = o.m_schema;
    return *this;
  }

  /****************************************************************************
   * Append a string value onto the end of the row in buffer (m_buffer).
   */
  void row::append (size_t i, std::string& value) {
    if (i >= static_cast<size_t>(m_schema->get_header().column_count())
        || not is_valid(i, value)
        || value.size() > static_cast<size_t>(m_schema->column_at(i).size()))
    {
      throw std::runtime_error{
        red::eno::get_msg(red::eno::ERRNO_VALUE_ERROR)};
    }

    /*** clear null value here ***/
    if (value == "null") value.clear();

    if ((m_schema->column_at(i).flag() & schema::FLAG_AUTO_INC)
        == schema::FLAG_AUTO_INC)
    {
      // get auto increment value
      int32_t ai_value = static_cast<int32_t>(m_schema->column_at(i)
          .auto_inc());
      // update auto increment value
      m_schema->column_at(i).auto_inc(static_cast<uint64_t>(ai_value + 1));

      if (m_schema->column_at(i).type() == schema::TYPE_FLOAT) {
        std::ostringstream oss;
        oss << std::setw(m_schema->column_at(i).size())
          << std::setprecision(m_schema->column_at(i).prec())
          << std::setfill('0') << ai_value;
        value = oss.str();
      } else if (m_schema->column_at(i).type() == schema::TYPE_INT) {
        std::ostringstream oss;
        oss << std::setw(m_schema->column_at(i).size())
          << std::setfill('0') << ai_value;
        value = oss.str();
      }
    } else if ((m_schema->column_at(i).flag() & schema::FLAG_PRIMARY)
        == schema::FLAG_PRIMARY
        || (m_schema->column_at(i).flag() & schema::FLAG_UNIQUE)
        == schema::FLAG_UNIQUE)
    {
      if (m_schema->column_at(i).type() == schema::TYPE_FLOAT) {
        float val = std::stof(value);
        std::ostringstream oss;
        oss << std::fixed << std::setw(m_schema->column_at(i).size())
          << std::setprecision(m_schema->column_at(i).prec())
          << std::setfill('0') << val;
        value = oss.str();
      } else if (m_schema->column_at(i).type() == schema::TYPE_INT) {
        int32_t val = std::stoi(value);
        std::ostringstream oss;
        oss << std::setw(m_schema->column_at(i).size())
          << std::setfill('0') << val;
        value = oss.str();
      }
    } else if (m_schema->column_at(i).type() == schema::TYPE_FLOAT) {
      try {
        float val = std::stof(value);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(m_schema->column_at(i).prec())
          << val;
        value = oss.str();
      } catch (...) {
        value.clear();
      }
    } else if (m_schema->column_at(i).type() == schema::TYPE_INT) {
      try {
        int32_t val = std::stoi(value);
        value = std::to_string(val);
      } catch (...) {
        value.clear();
      }
    } else if (m_schema->column_at(i).type() == schema::TYPE_BOOLEAN) {
      std::string val;
      for (auto ch : value) val += std::tolower(ch);
      if (val == "true") value = "1";
      else if (val == "false") value = "0";
      else value.clear();
    }

    m_schema->column_at(i).max_size(std::max(m_schema->column_at(i).max_size(),
          static_cast<int32_t>(value.size())));
    m_buffer.put_varchar(size(), value);
    size(m_buffer.index());
    int32_t index = red::tree::COLUMN_SIZE_POS + i * red::tree::COLUMN_SIZE_SZ;
    switch (m_schema->column_at(i).type()) {
      default: m_buffer.put(index, static_cast<uint8_t>(value.size()));
    }
  }

  /****************************************************************************
   *
   */
  red::buffer row::backing () {
    m_buffer.index(size());
    m_buffer.trip();
    return m_buffer;
  }

  /****************************************************************************
   * Get the index (in the vector> of the column with the corresponding name.
   */
  size_t row::column_pos (std::string const& name) {
    try { return m_schema->column_pos(name); } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void row::init (red::row& rw) {
    clear();
    m_buffer.put_int16(red::tree::ROW_PREV_POS,
        rw.m_buffer.get_int16(red::tree::ROW_PREV_POS));
    m_buffer.put_int16(red::tree::ROW_NEXT_POS,
        rw.m_buffer.get_int16(red::tree::ROW_NEXT_POS));
  }

  /****************************************************************************
   *
   */
  void row::copy_row (red::buffer& src) {
    m_buffer.clear();
    // get size of row (src)
    int32_t size = static_cast<int32_t>(src.get_int16());
    src.index(src.index() - red::tree::ROW_SIZE_SZ);
    // copy contents of row (src)
    for (int32_t i = 0; i < size; ++i)
      m_buffer.put(src.get());
    m_buffer.trip();
  }

  /****************************************************************************
   * Make foreign-key that corresponds to name.
   */
  std::string row::make_foreign_key (std::string const& name) {
    try {
      auto& fgn = m_schema->get_foreign(name);
      std::string key;
      for (auto& name : fgn.col_name_list()) {
        auto i = m_schema->column_pos(name);
        auto& col = m_schema->get_column_list()[i];
        std::ostringstream value;
        if (col.type() == schema::TYPE_FLOAT)
          value << std::right << std::setfill('0') << std::setw(col.size())
            << std::setprecision(col.prec()) << value_at(i);
        else if (col.type() == schema::TYPE_INT)
          value << std::right << std::setfill('0') << std::setw(col.size())
            << value_at(i);
        else
          value << std::left << std::setfill(' ') << std::setw(col.size())
            << value_at(i);
        key += value.str();
      }
      return key;
    } catch (...) { throw; }
  }

  /****************************************************************************
   * Make index key that corresponds to name.
   */
  std::string row::make_key (std::string const& name) {
    try {
      auto& ndx = m_schema->get_index(name);
      std::string key;
      for (auto& name : ndx.col_name_list()) {
        auto i = m_schema->column_pos(name);
        auto& col = m_schema->get_column_list()[i];
        std::ostringstream value;
        if (col.type() == schema::TYPE_FLOAT)
          value << std::right << std::setfill('0') << std::setw(col.size())
            << std::setprecision(col.prec()) << value_at(i);
        else if (col.type() == schema::TYPE_INT)
          value << std::right << std::setfill('0') << std::setw(col.size())
            << value_at(i);
        else
          value << std::left << std::setfill(' ') << std::setw(col.size())
            << value_at(i);
        key += value.str();
      }
      return key;
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  bool row::verify_value (size_t const i, std::string const& value) {
    return is_valid(i, value);
  }

  /****************************************************************************
   * Get the value of the column at the corresponding index (n).
   */
  std::string row::value_at (int32_t const n) {
    int32_t position {red::tree::COLUMN_SIZE_POS + m_schema->get_header()
    .column_count() * red::tree::COLUMN_SIZE_SZ};
    m_buffer.index(red::tree::COLUMN_SIZE_POS);
    for (int32_t i = 0; i < n; ++i)
      position += static_cast<int32_t>(m_buffer.get());
    int32_t size = static_cast<int32_t>(m_buffer.get());
    return m_buffer.get_varchar(position, size);
  }

  /****************************************************************************
   * Get the value of the column with the corresponding name.
   */
  std::string row::value_at (std::string const& name) {
    size_t n = m_schema->column_pos(name);
    int32_t position {red::tree::COLUMN_SIZE_POS + m_schema->get_header()
    .column_count() * red::tree::COLUMN_SIZE_SZ};
    m_buffer.index(red::tree::COLUMN_SIZE_POS);
    for (size_t i = 0; i < n; ++i)
      position += static_cast<int32_t>(m_buffer.get());
    int32_t size = static_cast<int32_t>(m_buffer.get());
    return m_buffer.get_varchar(position, size);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS ROW: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Initialize the row buffer
   */
  void row::init () {
    int32_t row_size = red::tree::COLUMN_SIZE_POS + m_schema->get_header()
      .column_count() * red::tree::COLUMN_SIZE_SZ;
    m_buffer.clear();
    m_buffer.put_int16(red::tree::ROW_SIZE_POS, row_size);
    m_buffer.put_int16(red::tree::ROW_PREV_POS, static_cast<int16_t>(0));
    m_buffer.put_int16(red::tree::ROW_NEXT_POS, static_cast<int16_t>(0));
  }

  /****************************************************************************
   * Is value a valid date?
   */
  bool row::is_date_valid (std::string const& value) {
    // Format: 'YYYY-MM-DD'
    // Range: '1000-01-01' to '9999-12-31'
    std::string const begin {"1000-01-01"};
    struct tm tm_begin { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0L, nullptr };
    (void) strptime(begin.c_str(), "%Y-%02m-%02d", &tm_begin);
    time_t time_begin = mktime(&tm_begin);
    std::string const end {"9999-12-31"};
    struct tm tm_end { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0L, nullptr };
    (void) strptime(end.c_str(), "%Y-%02m-%02d", &tm_end);
    time_t time_end = mktime(&tm_end);
    struct tm tm_value { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0L, nullptr };
    (void) strptime(value.c_str(), "%Y-%02m-%02d", &tm_value);
    time_t time_value = mktime(&tm_value);
    return (time_begin <= time_value && time_value <= time_end);
  }

  /****************************************************************************
   * Is value a valid datetime?
   */
  bool row::is_datetime_valid (std::string const& value) {
    // Format: 'YYYY-MM-DD HH:MM:SS'
    // Range: '1000-01-01 00:00:00' to '9999-12-31 23:59:59'
    std::string const begin {"1000-01-01 00:00:00"};
    struct tm tm_begin { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0L, nullptr };
    (void) strptime(begin.c_str(), "%Y-%02m-%02d", &tm_begin);
    time_t time_begin = mktime(&tm_begin);
    std::string const end {"9999-12-31 23:59:59"};
    struct tm tm_end { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0L, nullptr };
    (void) strptime(end.c_str(), "%Y-%02m-%02d %02H:%02M:%02S", &tm_end);
    time_t time_end = mktime(&tm_end);
    struct tm tm_value;
    (void) strptime(value.c_str(), "%Y-%02m-%02d %02H:%02M:%02S", &tm_value);
    time_t time_value = mktime(&tm_value);
    return (time_begin <= time_value && time_value <= time_end);
  }

  /****************************************************************************
   * Is value a valid enum?
   */
  bool row::is_enum_valid (size_t i, std::string const& value) {
    for (std::string e : m_schema->column_at(i).enum_list())
      if (value == e) return true;
    return false;
  }

  /****************************************************************************
   * Is value a valid timestamp?
   */
  bool row::is_timestamp_valid (std::string const& value) {
    // Format: 'YYYY-MM-DD HH:MM:SS'
    // Range: '1970-01-01 00:00:01' UTC to '2038-01-19 03:14:07' UTC
    std::string const begin {"1970-01-01 00:00:01"};
    struct tm tm_begin { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0L, nullptr };
    (void) strptime(begin.c_str(), "%Y-%02m-%02d", &tm_begin);
    time_t time_begin = mktime(&tm_begin);
    std::string const end {"2038-01-19 03:14:07"};
    struct tm tm_end { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0L, nullptr };
    (void) strptime(end.c_str(), "%Y-%02m-%02d %02H:%02M:%02S", &tm_end);
    time_t time_end = mktime(&tm_end);
    struct tm tm_value;
    (void) strptime(value.c_str(), "%Y-%02m-%02d %02H:%02M:%02S", &tm_value);
    time_t time_value = mktime(&tm_value);
    return (time_begin <= time_value && time_value <= time_end);
  }

  /****************************************************************************
   * Is the type of the value the right type? Does the type (T) of parameter
   * (value) correspond to the type of the column at that position in the row?
   */
  bool row::is_valid (size_t i, std::string const& value) {
    // is column-index greater and number of columns
    if (i >= static_cast<size_t>(m_schema->get_header().column_count()))
      return false;
    // check value for compliance with NOT-NULL flag
    if (not not_null_compliant(i, value)) return false;
    // if value is null return true
    if (to_lower(value) == "null") return true;
    // is value size greater-than column size
    if (value.size() > static_cast<size_t>(m_schema->column_at(i).size()))
      return false;

    switch (m_schema->column_at(i).type()) {
      case schema::TYPE_BOOLEAN: return is_boolean_valid(value);
      case schema::TYPE_CHAR:
      case schema::TYPE_BINARY:
      case schema::TYPE_VARCHAR: return true;
      case schema::TYPE_ENUM: return is_enum_valid(i, value);
      case schema::TYPE_DATE: return is_date_valid(value);
      case schema::TYPE_DATETIME: return is_datetime_valid(value);
      case schema::TYPE_TIMESTAMP: return is_timestamp_valid(value);
      case schema::TYPE_FLOAT: return is_float_valid(value);
      case schema::TYPE_INT: return is_integer_valid(value);
      default:;
    }

    return false;
  }

  /****************************************************************************
   * If value is null and NOT NULL flag is set, return false else return true.
   */
  bool row::not_null_compliant (size_t i, std::string const& value) {
    if ((m_schema->column_at(i).flag() & schema::FLAG_AUTO_INC)
        != schema::FLAG_AUTO_INC)
    {
      if ((m_schema->column_at(i).flag() & schema::FLAG_NOT_NULL)
          == schema::FLAG_NOT_NULL)
      {
        if (to_lower(value) == "null") return false;
      }
    }
    return true;
  }

} // namespace


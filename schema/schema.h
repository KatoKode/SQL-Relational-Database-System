//-----------------------------------------------------------------------------
// Desc: Schema library
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

#ifndef SCHEMA_H
#define SCHEMA_H 1

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <json-glib/json-glib.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include "eno.h"
#include "logger.h"
#include "bcalc.h"

#undef SCHEMA_DEBUG

namespace red::schema {

  std::string const PK_NDX {"NDX_000"};
  std::string const NDX_ {"NDX_"};
  std::string const FGN_ {"FGN_"};
  std::string const REF_ {"REF_"};
  std::string const COL_ {"COL_"};

  int32_t const FLAG_AUTO_INC   = 0x1;
  int32_t const FLAG_FOREIGN    = 0x2;
  int32_t const FLAG_NOT_NULL   = 0x4;
  int32_t const FLAG_PRIMARY    = 0x8;
  int32_t const FLAG_UNIQUE     = 0x10;
  int32_t const FLAG_UNSIGNED   = 0x20;
  int32_t const FLAG_ZEROFILL   = 0x40;
  int32_t const FLAG_MULTIPLE   = 0x80;

  int32_t const TYPE_BOOLEAN    = 1;
  int32_t const TYPE_CHAR       = 2;
  int32_t const TYPE_BINARY     = 3;
  int32_t const TYPE_DATE       = 4;
  int32_t const TYPE_DATETIME   = 5;
  int32_t const TYPE_ENUM       = 6;
  int32_t const TYPE_FLOAT      = 7;
  int32_t const TYPE_FOREIGN    = 8;
  int32_t const TYPE_INT        = 9;
  int32_t const TYPE_PRIMARY    = 10;
  int32_t const TYPE_SECONDARY  = 11;
  int32_t const TYPE_TIMESTAMP  = 12;
  int32_t const TYPE_VARCHAR    = 13;

  class column;
  class schema;
  class manager;

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS REFERENCE
   */
  class reference {
    public:
      reference () { }

      reference (std::string const& name,
          std::string const& db_name,
          std::string const& tbl_name,
          std::string const& fgn_name) :
        m_name {name},
        m_db_name {db_name},
        m_tbl_name {tbl_name},
        m_fgn_name {fgn_name}
      { }

      reference (reference const& o) :
        m_name {o.m_name},
        m_db_name {o.m_db_name},
        m_tbl_name {o.m_tbl_name},
        m_fgn_name {o.m_fgn_name}
      { }

      reference (reference&& o) :
        m_name {o.m_name},
        m_db_name {o.m_db_name},
        m_tbl_name {o.m_tbl_name},
        m_fgn_name {o.m_fgn_name}
      { }

      reference& operator=(reference const&);
      reference& operator=(reference&&);

      void name (std::string const& value) { m_name = value; }
      void db_name (std::string const& value) { m_db_name = value; }
      void tbl_name (std::string const& value) { m_tbl_name = value; }
      void fgn_name (std::string const& value) { m_fgn_name = value; }

      std::string name () const { return m_name; }
      std::string db_name () const { return m_db_name; }
      std::string tbl_name () const { return m_tbl_name; }
      std::string fgn_name () const { return m_fgn_name; }
    private:
      std::string m_name;
      std::string m_db_name;
      std::string m_tbl_name;
      std::string m_fgn_name;

      void get_name (JsonObject*);
      void get_db_name (JsonObject*);
      void get_tbl_name (JsonObject*);
      void get_fgn_name (JsonObject*);

      void put_name (JsonBuilder*);
      void put_db_name (JsonBuilder*);
      void put_tbl_name (JsonBuilder*);
      void put_fgn_name (JsonBuilder*);

      void from_json_object (JsonNode*);
      void to_json_object (JsonBuilder*);

      friend class column;
  };

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream&, red::schema::reference&);
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS COLUMN
   */
  class column {
    public:
      column () :
        m_flag {0},
        m_prec {0},
        m_size {0},
        m_type {0},
        m_max_size {0},
        m_auto_inc {0L}
      { }

      // For Types: DATE, DATETIME and TIMESTAMP
      column (int32_t flag,
          int32_t type,
          std::string const& name) :
        m_flag {flag},
        m_prec {0},
        m_size { (type == red::schema::TYPE_DATE ? 10 : 19) },
        m_type {type},
        m_max_size {0},
        m_auto_inc {0L},
        m_name {name}
      { }

      // For Types: BOOLEAN, CHAR, BINARY, INT and VARCHAR
      column (int32_t flag,
          int32_t size,
          int32_t type,
          std::string const& name) :
        m_flag {flag},
        m_prec {0},
        m_size {size},
        m_type {type},
        m_max_size {0},
        m_auto_inc {0L},
        m_name {name}
      { }

      // For Type: FLOAT
      column (int32_t flag,
          int32_t prec,
          int32_t size,
          int32_t type,
          std::string const& name) :
        m_flag {flag},
        m_prec {prec},
        m_size {size},
        m_type {type},
        m_max_size {0},
        m_auto_inc {0L},
        m_name {name}
      { }

      // For Type: ENUM
      column (int32_t flag,
          int32_t size,
          int32_t type,
          std::string const& name,
          std::vector<std::string>& _enum) :
        m_flag {flag},
        m_prec {0},
        m_size {size},
        m_type {type},
        m_max_size {0},
        m_auto_inc {0L},
        m_name {name},
        m_enum {_enum}
      { }

      column (column const& o) :
        m_flag {o.m_flag},
        m_prec {o.m_prec},
        m_size {o.m_size},
        m_type {o.m_type},
        m_max_size {o.m_max_size},
        m_auto_inc {o.m_auto_inc},
        m_name {o.m_name},
        m_enum {o.m_enum},
        m_reference_list {o.m_reference_list}
      { }


      column (column&& o) :
        m_flag {o.m_flag},
        m_prec {o.m_prec},
        m_size {o.m_size},
        m_type {o.m_type},
        m_max_size {o.m_max_size},
        m_auto_inc {o.m_auto_inc},
        m_name {o.m_name},
        m_enum {o.m_enum},
        m_reference_list {o.m_reference_list}
      { }

      column& operator=(column const&);
      column& operator=(column&&);

      bool operator==(std::string const& name) const {
        return (m_name == name);
      }

      bool operator!=(std::string const& name) const {
        return !(m_name == name);
      }

      bool operator<(std::string const& name) const {
        return (m_name < name);
      }

      bool operator>(std::string const& name) const {
        return (m_name > name);
      }

      void flag (int32_t value) { m_flag = value; }
      void prec (int32_t value) { m_prec = value; }
      void size (int32_t value) { m_size = value; }
      void type (int32_t value) { m_type = value; }
      void max_size (int32_t value) { m_max_size = value; }
      void auto_inc (uint64_t value) { m_auto_inc = value; }
      void name (std::string const& value) { m_name = value; }
      void enum_list (std::vector<std::string> const& list) { m_enum = list; }
      void reference_list (std::vector<red::schema::reference> const& list) {
        m_reference_list = list;
      }

      int32_t flag () const { return m_flag; }
      int32_t prec () const { return m_prec; }
      int32_t size () const { return m_size; }
      int32_t type () const { return m_type; }
      int32_t max_size () const { return m_max_size; }
      uint64_t auto_inc () const { return m_auto_inc; }
      std::string name () const { return m_name; }
      std::vector<std::string>& enum_list () { return m_enum; }
      std::vector<red::schema::reference>& reference_list () {
        return m_reference_list;
      }
    private:
      int32_t       m_flag;
      int32_t       m_prec; // precision (decimal places)
      int32_t       m_size; // size in characters
      int32_t       m_type;
      int32_t       m_max_size; // size of largest value
      uint64_t      m_auto_inc; // auto increment
      std::string   m_name;
      std::vector<std::string>    m_enum;
      std::vector<red::schema::reference>  m_reference_list;

      void get_flag (JsonObject*);
      void get_prec (JsonObject*);
      void get_size (JsonObject*);
      void get_type (JsonObject*);
      void get_max_size (JsonObject*);
      void get_auto_inc (JsonObject*);
      void get_name (JsonObject*);
      void get_enum (JsonObject*);
      void get_reference_list (JsonObject*);

      void put_flag (JsonBuilder*);
      void put_prec (JsonBuilder*);
      void put_size (JsonBuilder*);
      void put_type (JsonBuilder*);
      void put_max_size (JsonBuilder*);
      void put_auto_inc (JsonBuilder*);
      void put_name (JsonBuilder*);
      void put_enum (JsonBuilder*);
      void put_reference_list (JsonBuilder*);

      void from_json_object (JsonNode*);
      void to_json_object (JsonBuilder*, int32_t);

      friend class red::schema::schema;
      friend class red::schema::manager;
  };

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream&, column&);
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS HEADER
   */
  class header {
    public:
      header () { }

      header (std::string db_name,
          std::string tbl_name,
          int32_t flag,
          int32_t block_size,
          int32_t index_count,
          int32_t foreign_count,
          int32_t column_count,
          off_t next_offset,
          off_t data_offset,
          off_t data_root) :
        m_db_name {db_name},
        m_tbl_name {tbl_name},
        m_flag {flag},
        m_block_size {block_size},
        m_index_count {index_count},
        m_foreign_count {foreign_count},
        m_column_count {column_count},
        m_block_count {0L},
        m_row_count {0L},
        m_generation {0L},
        m_next_offset {next_offset},
        m_data_offset {data_offset},
        m_data_root {data_root},
        m_free_head {-1L},
        m_free_tail {-1L}
      { }

      header (header const& o) :
        m_db_name {o.m_db_name},
        m_tbl_name {o.m_tbl_name},
        m_flag {o.m_flag},
        m_block_size {o.m_block_size},
        m_index_count {o.m_index_count},
        m_foreign_count {o.m_foreign_count},
        m_column_count {o.m_column_count},
        m_block_count {o.m_block_count},
        m_row_count {o.m_row_count},
        m_generation {o.m_generation},
        m_next_offset {o.m_next_offset},
        m_data_offset {o.m_data_offset},
        m_data_root {o.m_data_root},
        m_free_head {o.m_free_head},
        m_free_tail {o.m_free_tail}
      { }


      header (header&& o) :
        m_db_name {o.m_db_name},
        m_tbl_name {o.m_tbl_name},
        m_flag {o.m_flag},
        m_block_size {o.m_block_size},
        m_index_count {o.m_index_count},
        m_foreign_count {o.m_foreign_count},
        m_column_count {o.m_column_count},
        m_block_count {o.m_block_count},
        m_row_count {o.m_row_count},
        m_generation {o.m_generation},
        m_next_offset {o.m_next_offset},
        m_data_offset {o.m_data_offset},
        m_data_root {o.m_data_root},
        m_free_head {o.m_free_head},
        m_free_tail {o.m_free_tail}
      { }

      header& operator=(header const&);
      header& operator=(header&&);

      void db_name (std::string const& value) { m_db_name = value; }
      void tbl_name (std::string const& value) { m_tbl_name = value; }
      void flag (int32_t value) { m_flag = value; }
      void block_size (int32_t value) { m_block_size = value; }
      void index_count (int32_t value) { m_index_count = value; }
      void foreign_count (int32_t value) { m_foreign_count = value; }
      void column_count (int32_t value) { m_column_count = value; }
      void block_count (int64_t value) { m_block_count = value; }
      void row_count (int64_t value) { m_row_count = value; }
      void generation (int64_t value) { m_generation = value; }
      void next_offset (off_t value) { m_next_offset = value; }
      void data_offset (off_t value) { m_data_offset = value; }
      void data_root (off_t value) { m_data_root = value; }
      void free_head (off_t value) { m_free_head = value; }
      void free_tail (off_t value) { m_free_tail = value; }

      std::string db_name () const { return m_db_name; }
      std::string tbl_name () const { return m_tbl_name; }
      int32_t flag () const { return m_flag; }
      int32_t block_size () const { return m_block_size; }
      int32_t index_count () const { return m_index_count; }
      int32_t foreign_count () const { return m_foreign_count; }
      int32_t column_count () const { return m_column_count; }
      int64_t block_count () const { return m_block_count; }
      int64_t row_count () const { return m_row_count; }
      int64_t generation () const { return m_generation; }
      off_t next_offset () const { return m_next_offset; }
      off_t data_offset () const { return m_data_offset; }
      off_t data_root () const { return m_data_root; }
      off_t free_head () const { return m_free_head; }
      off_t free_tail () const { return m_free_tail; }
    private:
      std::string   m_db_name;
      std::string   m_tbl_name;
      int32_t       m_flag;
      int32_t       m_block_size;
      int32_t       m_index_count;
      int32_t       m_foreign_count;
      int32_t       m_column_count;
      int64_t       m_block_count;
      int64_t       m_row_count;
      int64_t       m_generation;
      off_t         m_next_offset;
      off_t         m_data_offset;
      off_t         m_data_root;
      off_t         m_free_head;
      off_t         m_free_tail;

      void get_db_name (JsonObject*);
      void get_tbl_name (JsonObject*);
      void get_table_id (JsonObject*);
      void get_flag (JsonObject*);
      void get_block_size (JsonObject*);
      void get_index_count (JsonObject*);
      void get_foreign_count (JsonObject*);
      void get_column_count (JsonObject*);
      void get_block_count (JsonObject*);
      void get_row_count (JsonObject*);
      void get_generation (JsonObject*);
      void get_next_offset (JsonObject*);
      void get_data_offset (JsonObject*);
      void get_data_root (JsonObject*);
      void get_free_head (JsonObject*);
      void get_free_tail (JsonObject*);

      void put_db_name (JsonBuilder*);
      void put_tbl_name (JsonBuilder*);
      void put_table_id (JsonBuilder*);
      void put_flag (JsonBuilder*);
      void put_block_size (JsonBuilder*);
      void put_index_count (JsonBuilder*);
      void put_foreign_count (JsonBuilder*);
      void put_column_count (JsonBuilder*);
      void put_block_count (JsonBuilder*);
      void put_row_count (JsonBuilder*);
      void put_generation (JsonBuilder*);
      void put_next_offset (JsonBuilder*);
      void put_data_offset (JsonBuilder*);
      void put_data_root (JsonBuilder*);
      void put_free_head (JsonBuilder*);
      void put_free_tail (JsonBuilder*);

      void from_json_object (JsonNode*);
      void to_json_object (JsonBuilder*);

      friend class red::schema::schema;
      friend class red::schema::manager;
  };

  /***************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream&, red::schema::header&);
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS INDEX
   */
  class index {
    public:
      index () { }

      index (int32_t flag,
          int32_t order,
          int32_t key_size,
          std::string const& name,
          std::vector<std::string>& col_name_list) :
        m_flag {flag},
        m_order {order},
        m_key_size {key_size},
        m_block_count {0L},
        m_generation {0L},
        m_root {-1L},
        m_name {name},
        m_col_name_list {col_name_list}
      { }

      index (index const& o) :
        m_flag {o.m_flag},
        m_order {o.m_order},
        m_key_size {o.m_key_size},
        m_block_count {o.m_block_count},
        m_generation {o.m_generation},
        m_root {o.m_root},
        m_name {o.m_name},
        m_col_name_list {o.m_col_name_list}
      { }

      index (index&& o) :
        m_flag {o.m_flag},
        m_order {o.m_order},
        m_key_size {o.m_key_size},
        m_block_count {o.m_block_count},
        m_generation {o.m_generation},
        m_root {o.m_root},
        m_name {o.m_name},
        m_col_name_list {o.m_col_name_list}
      { }

      index& operator=(const index&);
      index& operator=(index&&);

      void flag (int32_t value) { m_flag = value; }
      void order (int32_t value) { m_order = value; }
      void key_size (int32_t value) { m_key_size = value; }
      void block_count (int64_t value) { m_block_count = value; }
      void generation (int64_t value) { m_generation = value; }
      void root (off_t value) { m_root = value; }
      void name (std::string const& value) { m_name = value; }
      void col_name_list (std::vector<std::string> const& list) {
        m_col_name_list = list;
      }

      int32_t flag () const { return m_flag; }
      int32_t order () const { return m_order; }
      int32_t key_size () const { return m_key_size; }
      int64_t block_count () const { return m_block_count; }
      int64_t generation () const { return m_generation; }
      off_t root () const { return m_root; }
      std::string name () const { return m_name; }
      std::vector<std::string>& col_name_list () { return m_col_name_list; }
    private:
      int32_t     m_flag;
      int32_t     m_order;
      int32_t     m_key_size;
      int64_t     m_block_count;
      int64_t     m_generation;
      off_t       m_root;
      std::string m_name;
      std::vector<std::string>  m_col_name_list;

      void get_flag (JsonObject*);
      void get_order (JsonObject*);
      void get_key_size (JsonObject*);
      void get_block_count (JsonObject*);
      void get_generation (JsonObject*);
      void get_root (JsonObject*);
      void get_name (JsonObject*);
      void get_col_name_list (JsonObject*);

      void put_flag (JsonBuilder*);
      void put_order (JsonBuilder*);
      void put_key_size (JsonBuilder*);
      void put_block_count (JsonBuilder*);
      void put_generation (JsonBuilder*);
      void put_root (JsonBuilder*);
      void put_name (JsonBuilder*);
      void put_col_name_list (JsonBuilder*);

      void from_json_object (JsonNode*);
      void to_json_object (JsonBuilder*);

      friend class schema;
      friend class manager;
  };

  /***************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream&, red::schema::index&);
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS FOREIGN KEY
   */
  class foreign {
    public:
      foreign () { }

      foreign (
          std::string const& name,
          std::string const& db_name,
          std::string const& tbl_name) :
        m_name {name},
        m_db_name {db_name},
        m_tbl_name {tbl_name}
      { }

      foreign (
          std::string const& name,
          std::string const& db_name,
          std::string const& tbl_name,
          std::vector<std::string> const& col_name_list) :
        m_name {name},
        m_db_name {db_name},
        m_tbl_name {tbl_name},
        m_col_name_list {col_name_list}
      { }

      foreign (foreign const& o) :
        m_name {o.m_name},
        m_db_name {o.m_db_name},
        m_tbl_name {o.m_tbl_name},
        m_col_name_list {o.m_col_name_list}
      { }

      foreign (foreign&& o) :
        m_name {o.m_name},
        m_db_name {o.m_db_name},
        m_tbl_name {o.m_tbl_name},
        m_col_name_list {o.m_col_name_list}
      { }

      foreign& operator=(const foreign&);
      foreign& operator=(foreign&&);

      void name (std::string const& value) { m_name = value; }
      void db_name (std::string const& value) { m_db_name = value; }
      void tbl_name (std::string const& value) { m_tbl_name = value; }
      void col_name_list (std::vector<std::string> const& list) {
        m_col_name_list = list;
      }

      std::string name () const { return m_name; }
      std::string db_name () const { return m_db_name; }
      std::string tbl_name () const { return m_tbl_name; }
      std::vector<std::string>& col_name_list () { return m_col_name_list; }
    private:
      std::string m_name;
      std::string m_db_name;    // name of database where foreign table exists
      std::string m_tbl_name;   // name of foreign table
      std::vector<std::string>  m_col_name_list; // local column name(s)

      void get_name (JsonObject*);
      void get_db_name (JsonObject*);
      void get_tbl_name (JsonObject*);
      void get_col_name_list (JsonObject*);

      void put_name (JsonBuilder*);
      void put_db_name (JsonBuilder*);
      void put_tbl_name (JsonBuilder*);
      void put_col_name_list (JsonBuilder*);

      void from_json_object (JsonNode*);
      void to_json_object (JsonBuilder*);

      friend class schema;
      friend class manager;
  };

  /***************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream&, red::schema::foreign&);
#endif

  /***************************************************************************
   * Predicate for std::find_if() that compares id values.
   */
  template<typename T>
    class id_predicate {
      public:
        explicit id_predicate (int32_t id) : m_id {id}
        { }

        bool operator () (const T& t) const {
          return (m_id == t.id ());
        }
      private:
        int32_t             m_id;
    };

  /***************************************************************************
   * Predicate for std::find_if() that compares name values.
   */
  template<typename T>
    class name_predicate {
      public:
        explicit name_predicate (std::string const& name) : m_name {name}
        { }

        bool operator () (const T& t) const {
          return (m_name == t.name ());
        }
      private:
        std::string const   m_name;
    };

  /***************************************************************************
   * CLASS NAME
   */
  template<typename T>
    class name {
      public:
        name (std::string const& prefix, std::vector<T>& list)
          : m_prefix {prefix}, m_list {list}
        { }

        std::string make ();
      private:
        std::string   m_prefix;
        std::vector<T>  m_list;
    };

  /****************************************************************************
   *
   */
  template<typename T>
  std::string name<T>::make () {
    int32_t id {0};
    std::ostringstream name;
    while (true) {
      name << m_prefix << std::setw(3) << std::setfill('0') << id++;
      auto iter = std::find_if(m_list.begin(), m_list.end(),
          [&](auto& ent){return(ent.name() == name.str());});
      if (iter == m_list.end()) break;
      name.str("");
    }
    return name.str();
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS SCHEMA
   */
  class schema {
    public:
      schema () { }

      schema (header& header, std::vector<column>& column,
          std::vector<index>& index)
        : m_header {header}, m_column {column}, m_index {index}
      { }

      schema (header& header, std::vector<column>& column,
          std::vector<index>& index, std::vector<foreign>& foreign)
        : m_header {header}, m_column {column}, m_index {index},
        m_foreign {foreign}
      { }

      schema (schema const&) = delete;

      schema (schema&& o)
        : m_header {o.m_header},
        m_column {o.m_column},
        m_index {o.m_index},
        m_foreign {o.m_foreign}
      { }

      schema& operator=(const schema&) = delete;
      schema& operator=(schema&&);

      red::schema::column& column_at (size_t const i) { return m_column[ i ]; }
      red::schema::foreign& foreign_at (int const i) { return m_foreign[ i ]; }
      red::schema::index& index_at (size_t const i) { return m_index[ i ]; }

      size_t column_pos (std::string const&);   // by name
      size_t foreign_pos (std::string const&);  // by name
      size_t index_pos (std::string const&);    // by name

      red::schema::column& get_column (std::string const&);    // by name
      red::schema::foreign& get_foreign (std::string const&);  // by name
      red::schema::index& get_index (std::string const&);      // by name
      red::schema::header& get_header () { return m_header; }

      std::vector<red::schema::column>& get_column_list () { return m_column; }
      std::vector<red::schema::foreign>& get_foreign_list () {
        return m_foreign;
      }
      std::vector<red::schema::index>& get_index_list () { return m_index; }

      std::string name () const;

      void remove_index(std::string const&);
    private:
      header  m_header;
      std::vector<red::schema::column>   m_column;
      std::vector<red::schema::index>    m_index;
      std::vector<red::schema::foreign>  m_foreign;

      friend class manager;
  };

  /***************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream&, schema&);
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS MANAGER
   */
  class manager {
    public:
      explicit manager (std::string const& path) : m_path {path} { }

      manager (manager const& mgr)
        : m_path {mgr.m_path}, m_schema {mgr.m_schema}
      { }

      ~manager ();

      manager (manager&& mgr) : m_schema {mgr.m_schema} { }

      manager& operator=(manager const&);
      manager& operator=(manager&&);

      /** name is the cannonical name "db_name.tbl_name" of the table.  */
      std::shared_ptr<schema> get_schema (std::string const& name) {
        auto iter = m_schema.find (name);
        return (iter != m_schema.end ()
            ? iter->second : load_schema (name));
      }

      /** temporary function */
      void add_schema (std::string const& name, schema* sch) {
        m_schema [ name ] = std::shared_ptr<schema>{sch};
      }

      void drop_schema (std::string const& name) {
        m_schema.erase(name);
      }

      /** name is the cannonical name "db_name.tbl_name" of the table.  */
      void save_schema (std::string const&);

      void save_schema (std::shared_ptr<red::schema::schema>);
    private:
      std::string     m_path; // full path to dbms directory

      std::map<std::string, std::shared_ptr<schema> >   m_schema;

      // JSON support https://developer.gnome.org/json-glib/stable/index.html

      /** name is the cannonical name "db_name.tbl_name" of the table.  */
      void insert (std::string name, schema* sch) {
        m_schema [ name ] = std::shared_ptr<schema>{sch};
      }

      /** name is the cannonical name "db_name.tbl_name" of the table.  */
      std::shared_ptr<schema> load_schema (std::string const& name);
  };

} // namespace

#endif  // SCHEMA_H


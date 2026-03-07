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

#include "schema.h"

namespace red::schema {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS REFERENCE: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS REFERENCE: Copy Assignment
   */
  reference& reference::operator=(reference const& o) {
    if (this == &o) return *this;
    m_name      = o.m_name;
    m_db_name   = o.m_db_name;
    m_tbl_name  = o.m_tbl_name;
    m_fgn_name  = o.m_fgn_name;
    return *this;
  }

  /****************************************************************************
   * CLASS REFERENCE: Move Assignment
   */
  reference& reference::operator=(reference&& o) {
    if (this == &o) return *this;
    m_name      = o.m_name;
    m_db_name   = o.m_db_name;
    m_tbl_name  = o.m_tbl_name;
    m_fgn_name  = o.m_fgn_name;
    return *this;
  }

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream& os, red::schema::reference& o) {
    os << "\tREFERENCE:\t" << o.name() << '\t' << o.db_name() << '\t'
      << o.tbl_name() << '\t' << o.fgn_name() << '\n';
    return os;
  }
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS REFERENCE: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void reference::get_name (JsonObject* object) {
    gchar const* value = nullptr;
    if ((value = json_object_get_string_member(object, "name")) == nullptr)
      return;
    m_name = value;
  }

  /****************************************************************************
   *
   */
  void reference::get_db_name (JsonObject* object) {
    gchar const* value = nullptr;
    if ((value = json_object_get_string_member(object, "db_name")) == nullptr)
      return;
    m_db_name = value;
  }

  /****************************************************************************
   *
   */
  void reference::get_tbl_name (JsonObject* object) {
    gchar const* value = nullptr;
    if ((value = json_object_get_string_member(object, "tbl_name")) == nullptr)
      return;
    m_tbl_name = value;
  }

  /****************************************************************************
   *
   */
  void reference::get_fgn_name (JsonObject* object) {
    gchar const* value = nullptr;
    if ((value = json_object_get_string_member(object, "fgn_name")) == nullptr)
      return;
    m_fgn_name = value;
  }

  /****************************************************************************
   *
   */
  void reference::from_json_object (JsonNode* node) {
    if (!JSON_NODE_HOLDS_OBJECT(node)) return;
    JsonObject* object = json_node_get_object(node);
    get_name(object);
    get_db_name(object);
    get_tbl_name(object);
    get_fgn_name(object);
  }

  /****************************************************************************
   *
   */
  void reference::put_name (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "name");
    (void) json_builder_add_string_value (builder,
        static_cast<gchar const*>(m_name.c_str()));
  }

  /****************************************************************************
   *
   */
  void reference::put_db_name (JsonBuilder* builder) {
    (void) json_builder_set_member_name(builder, "db_name");
    (void) json_builder_add_string_value(builder,
        static_cast<gchar const*>(m_db_name.c_str()));
  }

  /****************************************************************************
   *
   */
  void reference::put_tbl_name (JsonBuilder* builder) {
    (void) json_builder_set_member_name(builder, "tbl_name");
    (void) json_builder_add_string_value(builder,
        static_cast<gchar const*>(m_tbl_name.c_str()));
  }

  /****************************************************************************
   *
   */
  void reference::put_fgn_name (JsonBuilder* builder) {
    (void) json_builder_set_member_name(builder, "fgn_name");
    (void) json_builder_add_string_value(builder,
        static_cast<gchar const*>(m_fgn_name.c_str()));
  }

  /****************************************************************************
   *
   */
  void reference::to_json_object (JsonBuilder* builder) {
    json_builder_begin_object(builder);
    put_name(builder);
    put_db_name(builder);
    put_tbl_name(builder);
    put_fgn_name(builder);
    json_builder_end_object (builder);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS COLUMN: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS COLUMN: Copy Assignment Operator
   */
  column& column::operator=(column const& o) {
    if (this == &o) return *this;
    m_flag  = o.m_flag;
    m_prec  = o.m_prec;
    m_size  = o.m_size;
    m_type  = o.m_type;
    m_max_size  = o.m_max_size;
    m_auto_inc  = o.m_auto_inc;
    m_name  = o.m_name;
    m_enum  = o.m_enum;
    m_reference_list  = o.m_reference_list;
    return *this;
  }

  /***************************************************************************
   * CLASS COLUMN: Move Assignment Operator
   */
  column& column::operator=(column&& o) {
    if (this == &o) return *this;
    m_flag  = o.m_flag;
    m_prec  = o.m_prec;
    m_size  = o.m_size;
    m_type  = o.m_type;
    m_max_size  = o.m_max_size;
    m_auto_inc  = o.m_auto_inc;
    m_name  = o.m_name;
    m_enum  = o.m_enum;
    m_reference_list  = o.m_reference_list;
    return *this;
  }

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream& os, column& o) {
    os << "COLUMN:\n\tflag:\t" << "0x" << std::hex << o.flag() << std::dec
      << "\n\tprec:\t" << o.prec()
      << "\n\tsize:\t" << o.size()
      << "\n\ttype:\t" << o.type()
      << "\n\tmax size:\t" << o.max_size()
      << "\n\tauto inc:\t" << o.auto_inc()
      << "\n\tname:\t" << o.name()
      << "\n\tenums:";
    for (auto e : o.enum_list()) os << '\t' << e;
    os << '\n';
    for (auto r : o.reference_list()) os << r;
    os << '\n';
    return os;
  }
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS COLUMN: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void column::get_flag (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "flag");
    m_flag = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void column::get_prec (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "prec");
    m_prec = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void column::get_size (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "size");
    m_size = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void column::get_type (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "type");
    m_type = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void column::get_max_size (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "max_size");
    m_max_size = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void column::get_auto_inc (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "auto_inc");
    m_auto_inc = static_cast<uint64_t>(value);
  }

  /****************************************************************************
   *
   */
  void column::get_name (JsonObject* object) {
    gchar const* value = nullptr;
    if ((value = json_object_get_string_member (object, "name")) == nullptr)
      return;
    m_name = value;
  }

  /****************************************************************************
   *
   */
  void column::get_enum (JsonObject* object) {
    JsonArray* array = json_object_get_array_member(object, "enum");
    guint size = json_array_get_length(array);
    for (guint i = 0; i < size; ++i)
      m_enum.push_back(json_array_get_string_element(array, i));
  }

  /****************************************************************************
   *
   */
  void column::get_reference_list (JsonObject* object) {
    JsonArray* array = json_object_get_array_member(object, "ref_by");
    guint size = json_array_get_length(array);
    for (guint i = 0; i < size; ++i) {
      red::schema::reference ref;
      JsonNode* node = json_array_get_element(array, i);
      ref.from_json_object(node);
      m_reference_list.push_back(ref);
    }
  }

  /****************************************************************************
   *
   */
  void column::from_json_object (JsonNode* node) {
    if (!JSON_NODE_HOLDS_OBJECT (node)) return;
    JsonObject* object = json_node_get_object (node);
    get_flag (object);
    get_type (object);  /** get m_type before using m_type */
    if (m_type == red::schema::TYPE_FLOAT) get_prec (object);
    get_size (object);
    get_max_size (object);
    if ((m_flag & red::schema::FLAG_AUTO_INC) == red::schema::FLAG_AUTO_INC)
      get_auto_inc (object);
    get_name (object);
    if (m_type == red::schema::TYPE_ENUM) get_enum (object);
    get_reference_list(object);
  }

  /****************************************************************************
   *
   */
  void column::put_flag (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "flag");
    (void) json_builder_add_int_value (builder, static_cast<gint64>(m_flag));
  }

  /****************************************************************************
   *
   */
  void column::put_prec (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "prec");
    (void) json_builder_add_int_value (builder, static_cast<gint64>(m_prec));
  }

  /****************************************************************************
   *
   */
  void column::put_size (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "size");
    (void) json_builder_add_int_value (builder, static_cast<gint64>(m_size));
  }

  /****************************************************************************
   *
   */
  void column::put_type (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "type");
    (void) json_builder_add_int_value (builder, static_cast<gint64>(m_type));
  }

  /****************************************************************************
   *
   */
  void column::put_max_size (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "max_size");
    (void) json_builder_add_int_value (builder,
        static_cast<gint64>(m_max_size));
  }

  /****************************************************************************
   *
   */
  void column::put_auto_inc (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "auto_inc");
    (void) json_builder_add_int_value (builder,
        static_cast<gint64>(m_auto_inc));
  }

  /****************************************************************************
   *
   */
  void column::put_name (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "name");
    (void) json_builder_add_string_value (builder,
        static_cast<gchar const*>(m_name.c_str()));
  }

  /****************************************************************************
   *
   */
  void column::put_enum (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "enum");
    (void) json_builder_begin_array(builder);
    for (size_t i = 0; i < m_enum.size(); ++i)
      (void) json_builder_add_string_value (builder,
          static_cast<gchar const*>(m_enum [ i ].c_str()));
    (void) json_builder_end_array(builder);
  }

  /****************************************************************************
   *
   */
  void column::put_reference_list (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "ref_by");
    (void) json_builder_begin_array(builder);
    for (auto rb : m_reference_list) rb.to_json_object(builder);
    (void) json_builder_end_array(builder);
  }

  /****************************************************************************
   *
   */
  void column::to_json_object (JsonBuilder* builder, int32_t count) {
    char name [ 16 ];
    (void) sprintf(name, "column_%02d", count);
    (void) json_builder_set_member_name(builder, name);

    (void) json_builder_begin_object(builder);
    put_flag (builder);
    put_type (builder);
    if (m_type == red::schema::TYPE_FLOAT) put_prec(builder);
    put_size (builder);
    put_max_size (builder);
    if ((m_flag & red::schema::FLAG_AUTO_INC) == red::schema::FLAG_AUTO_INC)
      put_auto_inc(builder);
    put_name(builder);
    if (m_type == red::schema::TYPE_ENUM) put_enum(builder);
    put_reference_list(builder);
    (void) json_builder_end_object(builder);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS HEADER: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS HEADER: Copy Assignment Operator
   */
  header& header::operator=(header const& o) {
    if (this == &o) return *this;
    m_db_name         = o.m_db_name;
    m_tbl_name        = o.m_tbl_name;
    m_flag            = o.m_flag;
    m_block_size      = o.m_block_size;
    m_index_count     = o.m_index_count;
    m_foreign_count   = o.m_foreign_count;
    m_column_count    = o.m_column_count;
    m_block_count     = o.m_block_count;
    m_row_count       = o.m_row_count;
    m_generation      = o.m_generation;
    m_next_offset     = o.m_next_offset;
    m_data_offset     = o.m_data_offset;
    m_data_root       = o.m_data_root;
    m_free_head       = o.m_free_head;
    m_free_tail       = o.m_free_tail;
    return *this;
  }

  /***************************************************************************
   * CLASS HEADER: Move Assignment Operator
   */
  header& header::operator=(header&& o) {
    if (this == &o) return *this;
    m_db_name         = o.m_db_name;
    m_tbl_name        = o.m_tbl_name;
    m_flag            = o.m_flag;
    m_block_size      = o.m_block_size;
    m_index_count     = o.m_index_count;
    m_foreign_count   = o.m_foreign_count;
    m_column_count    = o.m_column_count;
    m_block_count     = o.m_block_count;
    m_row_count       = o.m_row_count;
    m_generation      = o.m_generation;
    m_next_offset     = o.m_next_offset;
    m_data_offset     = o.m_data_offset;
    m_data_root       = o.m_data_root;
    m_free_head       = o.m_free_head;
    m_free_tail       = o.m_free_tail;
    return *this;
  }

  /***************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream& os, red::schema::header& o) {
    os << "HEADER:\n\tdb_name:\t" << o.db_name()
      << "\n\ttbl_name:\t" << o.tbl_name()
      << std::hex << "\n\tflag:\t" << o.flag() << std::dec
      << "\n\tblock size:\t" << o.block_size()
      << "\n\tndx count:\t" << o.index_count()
      << "\n\tfgn count:\t" << o.foreign_count()
      << "\n\tcol count:\t" << o.column_count()
      << "\n\tblk count:\t" << o.block_count()
      << "\n\trow count:\t" << o.row_count()
      << "\n\tgeneration:\t" << o.generation()
      << "\n\tnext offset:\t" << o.next_offset()
      << "\n\tdata offset:\t" << o.data_offset()
      << "\n\tdata root:\t" << o.data_root()
      << "\n\tfree head:\t" << o.free_head()
      << "\n\tfree tail:\t" << o.free_tail();
    return os;
  }
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS HEADER: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void header::get_db_name (JsonObject* object) {
    gchar const* value = nullptr;
    if ((value = json_object_get_string_member (object, "db_name")) == nullptr)
      return;
    m_db_name = value;
  }

  /****************************************************************************
   *
   */
  void header::get_tbl_name (JsonObject* object) {
    gchar const* value = nullptr;
    if ((value = json_object_get_string_member (object,
            "tbl_name")) == nullptr) return;
    m_tbl_name = value;
  }

  /****************************************************************************
   *
   */
  void header::get_flag (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "flag");
    m_flag = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_block_size (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "block_size");
    m_block_size = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_index_count (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "index_count");
    m_index_count = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_foreign_count (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "foreign_count");
    m_foreign_count = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_column_count (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "column_count");
    m_column_count = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_block_count (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "block_count");
    m_block_count = static_cast<int64_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_row_count (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "row_count");
    m_row_count = static_cast<int64_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_generation (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "generation");
    m_generation = static_cast<int64_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_next_offset (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "next_offset");
    m_next_offset = static_cast<off_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_data_offset (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "data_offset");
    m_data_offset = static_cast<off_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_data_root (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "data_root");
    m_data_root = static_cast<off_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_free_head (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "free_head");
    m_free_head = static_cast<off_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::get_free_tail (JsonObject* object) {
    gint64 value = json_object_get_int_member (object, "free_tail");
    m_free_tail = static_cast<off_t>(value);
  }

  /****************************************************************************
   *
   */
  void header::from_json_object (JsonNode* node) {
    if (!JSON_NODE_HOLDS_OBJECT(node)) return;
    JsonObject* object = json_node_get_object(node);
    get_db_name(object);
    get_tbl_name(object);
    get_flag(object);
    get_block_size(object);
    get_index_count(object);
    get_foreign_count(object);
    get_column_count(object);
    get_block_count(object);
    get_row_count(object);
    get_generation(object);
    get_next_offset(object);
    get_data_offset(object);
    get_data_root(object);
    get_free_head(object);
    get_free_tail(object);
  }

  /****************************************************************************
   *
   */
  void header::put_db_name (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "db_name");
    json_builder_add_string_value (builder,
        static_cast<gchar const*>(m_db_name.c_str ()));
  }

  /****************************************************************************
   *
   */
  void header::put_tbl_name (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "tbl_name");
    json_builder_add_string_value (builder,
        static_cast<gchar const*>(m_tbl_name.c_str ()));
  }

  /****************************************************************************
   *
   */
  void header::put_flag (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "flag");
    json_builder_add_int_value (builder, static_cast<gint64>(m_flag));
  }

  /****************************************************************************
   *
   */
  void header::put_block_size (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "block_size");
    json_builder_add_int_value (builder,
        static_cast<gint64>(m_block_size));
  }

  /****************************************************************************
   *
   */
  void header::put_index_count (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "index_count");
    json_builder_add_int_value (builder, static_cast<gint64>(m_index_count));
  }

  /****************************************************************************
   *
   */
  void header::put_foreign_count (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "foreign_count");
    json_builder_add_int_value (builder, static_cast<gint64>(m_foreign_count));
  }

  /****************************************************************************
   *
   */
  void header::put_column_count (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "column_count");
    json_builder_add_int_value (builder, static_cast<gint64>(m_column_count));
  }

  /****************************************************************************
   *
   */
  void header::put_block_count (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "block_count");
    json_builder_add_int_value (builder, static_cast<gint64>(m_block_count));
  }

  /****************************************************************************
   *
   */
  void header::put_row_count (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "row_count");
    json_builder_add_int_value (builder, static_cast<gint64>(m_row_count));
  }

  /****************************************************************************
   *
   */
  void header::put_generation (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "generation");
    json_builder_add_int_value (builder, static_cast<gint64>(m_generation));
  }

  /****************************************************************************
   *
   */
  void header::put_next_offset (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "next_offset");
    json_builder_add_int_value (builder, static_cast<gint64>(m_next_offset));
  }

  /****************************************************************************
   *
   */
  void header::put_data_offset (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "data_offset");
    json_builder_add_int_value (builder, static_cast<gint64>(m_data_offset));
  }

  /****************************************************************************
   *
   */
  void header::put_data_root (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "data_root");
    json_builder_add_int_value (builder, static_cast<gint64>(m_data_root));
  }

  /****************************************************************************
   *
   */
  void header::put_free_head (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "free_head");
    json_builder_add_int_value (builder, static_cast<gint64>(m_free_head));
  }

  /****************************************************************************
   *
   */
  void header::put_free_tail (JsonBuilder* builder) {
    json_builder_set_member_name (builder, "free_tail");
    json_builder_add_int_value (builder, static_cast<gint64>(m_free_tail));
  }

  /****************************************************************************
   *
   */
  void header::to_json_object (JsonBuilder* builder) {
    json_builder_set_member_name(builder, "header");
    json_builder_begin_object(builder);
    put_db_name(builder);
    put_tbl_name(builder);
    put_flag(builder);
    put_block_size(builder);
    put_index_count(builder);
    put_foreign_count(builder);
    put_column_count(builder);
    put_block_count(builder);
    put_row_count(builder);
    put_generation(builder);
    put_next_offset(builder);
    put_data_offset(builder);
    put_data_root(builder);
    put_free_head(builder);
    put_free_tail(builder);
    json_builder_end_object(builder);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS INDEX: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS INDEX: Copy Assignment Operator
   */
  index& index::operator=(index const& o) {
    if (this == &o) return *this;
    m_flag          = o.m_flag;
    m_order         = o.m_order;
    m_key_size      = o.m_key_size;
    m_block_count   = o.m_block_count;
    m_generation    = o.m_generation;
    m_root          = o.m_root;
    m_name          = o.m_name;
    m_col_name_list = o.m_col_name_list;
    return *this;
  }

  /***************************************************************************
   * CLASS INDEX: Move Assignment Operator
   */
  index& index::operator=(index&& o) {
    if (this == &o) return *this;
    m_flag          = o.m_flag;
    m_order         = o.m_order;
    m_key_size      = o.m_key_size;
    m_block_count   = o.m_block_count;
    m_generation    = o.m_generation;
    m_root          = o.m_root;
    m_name          = o.m_name;
    m_col_name_list = o.m_col_name_list;
    return *this;
  }

  /***************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream& os, red::schema::index& o) {
    os << "INDEX:\n\tflag:\t" << std::hex << o.flag() << std::dec
      << "\n\torder:\t" << o.order()
      << "\n\tkey size:\t" << o.key_size()
      << "\n\tblk count:\t" << o.block_count()
      << "\n\tgeneration:\t" << o.generation()
      << "\n\troot:\t" << o.root()
      << "\n\tname:\t" << o.name()
      << "\n\tcol_name_list:\t";
    for (auto n : o.col_name_list()) os << n << '\t';
    os << '\n';
    return os;
  }
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS INDEX: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void index::get_flag (JsonObject* object) {
    gint64 value = json_object_get_int_member(object, "flag");
    m_flag = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void index::get_order (JsonObject* object) {
    gint64 value = json_object_get_int_member(object, "order");
    m_order = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void index::get_key_size (JsonObject* object) {
    gint64 value = json_object_get_int_member(object, "key_size");
    m_key_size = static_cast<int32_t>(value);
  }

  /****************************************************************************
   *
   */
  void index::get_block_count (JsonObject* object) {
    gint64 value = json_object_get_int_member(object, "block_count");
    m_block_count = static_cast<int64_t>(value);
  }

  /****************************************************************************
   *
   */
  void index::get_generation (JsonObject* object) {
    gint64 value = json_object_get_int_member(object, "generation");
    m_generation = static_cast<int64_t>(value);
  }

  /****************************************************************************
   *
   */
  void index::get_root (JsonObject* object) {
    gint64 value = json_object_get_int_member(object, "root");
    m_root = static_cast<std::fstream::off_type>(value);
  }

  /****************************************************************************
   *
   */
  void index::get_name (JsonObject* object) {
    gchar const* value;
    if ((value = json_object_get_string_member(object, "name")) == nullptr)
      return;
    m_name = value;
  }

  /****************************************************************************
   *
   */
  void index::get_col_name_list (JsonObject* object) {
    JsonArray* array = json_object_get_array_member(object, "col_name_list");
    guint size = json_array_get_length(array);
    for (guint i = 0; i < size; ++i) {
      gchar const* value = json_array_get_string_element(array, i);
      m_col_name_list.push_back(static_cast<std::string>(value));
    }
  }

  /****************************************************************************
   *
   */
  void index::from_json_object (JsonNode* node) {
    if (!JSON_NODE_HOLDS_OBJECT(node)) return;
    JsonObject* object = json_node_get_object(node);
    get_flag(object);
    get_order(object);
    get_key_size(object);
    get_block_count(object);
    get_block_count(object);
    get_generation(object);
    get_root(object);
    get_name(object);
    get_col_name_list(object);
  }

  /****************************************************************************
   *
   */
  void index::put_flag (JsonBuilder* builder) {
    json_builder_set_member_name(builder, "flag");
    json_builder_add_int_value(builder, static_cast<gint64>(m_flag));
  }

  /****************************************************************************
   *
   */
  void index::put_order (JsonBuilder* builder) {
    json_builder_set_member_name(builder, "order");
    json_builder_add_int_value(builder, static_cast<gint64>(m_order));
  }

  /****************************************************************************
   *
   */
  void index::put_key_size (JsonBuilder* builder) {
    json_builder_set_member_name(builder, "key_size");
    json_builder_add_int_value(builder, static_cast<gint64>(m_key_size));
  }

  /****************************************************************************
   *
   */
  void index::put_block_count (JsonBuilder* builder) {
    json_builder_set_member_name(builder, "block_count");
    json_builder_add_int_value(builder, static_cast<gint64>(m_block_count));
  }

  /****************************************************************************
   *
   */
  void index::put_generation (JsonBuilder* builder) {
    json_builder_set_member_name(builder, "generation");
    json_builder_add_int_value(builder, static_cast<gint64>(m_generation));
  }

  /****************************************************************************
   *
   */
  void index::put_root (JsonBuilder* builder) {
    json_builder_set_member_name(builder, "root");
    json_builder_add_int_value(builder, static_cast<gint64>(m_root));
  }

  /****************************************************************************
   *
   */
  void index::put_name (JsonBuilder* builder) {
    (void) json_builder_set_member_name(builder, "name");
    (void) json_builder_add_string_value(builder,
        static_cast<gchar const*>(m_name.c_str()));
  }

  /****************************************************************************
   *
   */
  void index::put_col_name_list (JsonBuilder* builder) {
    (void) json_builder_set_member_name(builder, "col_name_list");
    (void) json_builder_begin_array(builder);
    for (auto name : m_col_name_list)
      (void) json_builder_add_string_value(builder,
          static_cast<gchar const*>(name.c_str()));
    (void) json_builder_end_array(builder);
  }

  /****************************************************************************
   *
   */
  void index::to_json_object (JsonBuilder* builder) {
    json_builder_set_member_name(builder,
        static_cast<gchar const*>(m_name.c_str()));
    json_builder_begin_object(builder);
    put_flag(builder);
    put_order(builder);
    put_key_size(builder);
    put_block_count(builder);
    put_generation(builder);
    put_root(builder);
    put_name(builder);
    put_col_name_list(builder);
    json_builder_end_object(builder);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS FOREIGN: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS FOREIGN KEY: Copy Assignment Operator
   */
  foreign& foreign::operator=(foreign const& o) {
    if (this == &o) return *this;
    m_name        = o.m_name;
    m_db_name     = o.m_db_name;
    m_tbl_name    = o.m_tbl_name;
    m_col_name_list = o.m_col_name_list;
    return *this;
  }

  /***************************************************************************
   * CLASS FOREIGN KEY: Move Assignment Operator
   */
  foreign& foreign::operator=(foreign&& o) {
    if (this == &o) return *this;
    m_name        = o.m_name;
    m_db_name     = o.m_db_name;
    m_tbl_name    = o.m_tbl_name;
    m_col_name_list = o.m_col_name_list;
    return *this;
  }

  /***************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream& os, red::schema::foreign& o) {
    os << "FOREIGN:\n\tname:\t" << o.name()
      << "\n\tdb_name:\t" << o.db_name()
      << "\n\ttbl_name:\t" << o.tbl_name()
      << "\n\tcol_name_list:\t";
    for (auto n : o.col_name_list()) os << n << '\t';
    os << '\n';
    return os;
  }
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS FOREIGN: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void foreign::get_name (JsonObject* object) {
    gchar const* value = nullptr;
    if ((value = json_object_get_string_member(object, "name")) == nullptr)
      return;
    m_name = value;
  }

  /****************************************************************************
   *
   */
  void foreign::get_db_name (JsonObject* object) {
    gchar const* value = nullptr;
    if ((value = json_object_get_string_member(object, "db_name")) == nullptr)
      return;
    m_db_name = value;
  }

  /****************************************************************************
   *
   */
  void foreign::get_tbl_name (JsonObject* object) {
    gchar const* value = nullptr;
    if ((value = json_object_get_string_member(object, "tbl_name")) == nullptr)
      return;
    m_tbl_name = value;
  }

  /****************************************************************************
   *
   */
  void foreign::get_col_name_list (JsonObject* object) {
    JsonArray* array = json_object_get_array_member(object, "col_name_list");
    guint size = json_array_get_length(array);
    for (guint i = 0; i < size; ++i) {
      gchar const* value = json_array_get_string_element(array, i);
      m_col_name_list.push_back(std::string{value});
    }
  }

  /****************************************************************************
   *
   */
  void foreign::from_json_object (JsonNode* node) {
    if (!JSON_NODE_HOLDS_OBJECT(node)) return;
    JsonObject* object = json_node_get_object(node);
    get_name(object);
    get_db_name(object);
    get_tbl_name(object);
    get_col_name_list(object);
  }

  /****************************************************************************
   *
   */
  void foreign::put_name (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "name");
    (void) json_builder_add_string_value (builder,
        static_cast<gchar const*>(m_name.c_str()));
  }

  /****************************************************************************
   *
   */
  void foreign::put_db_name (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "db_name");
    (void) json_builder_add_string_value (builder,
        static_cast<gchar const*>(m_db_name.c_str()));
  }

  /****************************************************************************
   *
   */
  void foreign::put_tbl_name (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "tbl_name");
    (void) json_builder_add_string_value (builder,
        static_cast<gchar const*>(m_tbl_name.c_str()));
  }

  /****************************************************************************
   *
   */
  void foreign::put_col_name_list (JsonBuilder* builder) {
    (void) json_builder_set_member_name (builder, "col_name_list");
    (void) json_builder_begin_array(builder);
    for (auto cn : m_col_name_list)
      (void) json_builder_add_string_value (builder, cn.c_str());
    (void) json_builder_end_array(builder);
  }

  /****************************************************************************
   *
   */
  void foreign::to_json_object (JsonBuilder* builder) {
    json_builder_set_member_name(builder,
        static_cast<gchar const*>(m_name.c_str()));
    json_builder_begin_object(builder);
    put_name(builder);
    put_db_name(builder);
    put_tbl_name(builder);
    put_col_name_list(builder);
    json_builder_end_object (builder);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS SCHEMA: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS SCHEMA: Move Assignment Operator
   */
  schema& schema::operator=(schema&& o) {
    if (this == &o) return *this;
    m_header  = o.m_header;
    m_column  = o.m_column;
    m_index   = o.m_index;
    m_foreign = o.m_foreign;
    return *this;
  }

  /***************************************************************************
   * Get the position (in the vector) of the column with the corresponding
   * name.
   */
  size_t schema::column_pos (std::string const& name) {
    auto iter = std::find_if(m_column.begin(), m_column.end(),
        name_predicate<red::schema::column> {name});
    if (iter == m_column.end())
      throw std::runtime_error{
        red::eno::get_msg(red::eno::ERRNO_COLUMN_ERROR)};
    return static_cast<size_t>(iter - m_column.begin());
  }

  /***************************************************************************
   * Get the position (in the vector) of the foreign schema with the corre-
   * sponding name.
   */
  size_t schema::foreign_pos (std::string const& name) {
    auto iter = std::find_if(m_foreign.begin(), m_foreign.end(),
        name_predicate<red::schema::foreign> {name});
    if (iter == m_foreign.end())
      throw std::runtime_error{
        red::eno::get_msg(red::eno::ERRNO_FOREIGN_ERROR)};
    return static_cast<size_t>(iter - m_foreign.begin());
  }

  /***************************************************************************
   * Get the position (in the vector) of the index schema with the corre-
   * sponding name.
   */
  size_t schema::index_pos (std::string const& name) {
    auto iter = std::find_if(m_index.begin(), m_index.end(),
        name_predicate<red::schema::index> {name});
    if (iter == m_index.end())
      throw std::runtime_error{
        red::eno::get_msg(red::eno::ERRNO_INDEX_ERROR)};
    return static_cast<size_t>(iter - m_index.begin());
  }

  /***************************************************************************
   * Get reference to the column schema with the corresponding name.
   */
  red::schema::column& schema::get_column (std::string const& name) {
    auto iter = std::find_if(m_column.begin(), m_column.end(),
        name_predicate<red::schema::column> {name});
    if (iter == m_column.end())
      throw std::runtime_error{
        red::eno::get_msg(red::eno::ERRNO_COLUMN_ERROR)};
    return *iter;
  }

  /***************************************************************************
   * Get reference to the foreign schema with the corresponding name.
   */
  red::schema::foreign& schema::get_foreign (std::string const& name) {
    auto iter = std::find_if(m_foreign.begin(), m_foreign.end(),
        name_predicate<red::schema::foreign> {name});
    if (iter == m_foreign.end())
      throw std::runtime_error{
        red::eno::get_msg(red::eno::ERRNO_FOREIGN_ERROR)};
    return *iter;
  }

  /***************************************************************************
   * Get reference to the index schema with the corresponding name.
   */
  red::schema::index& schema::get_index (std::string const& name) {
    auto iter = std::find_if(m_index.begin(), m_index.end(),
        name_predicate<red::schema::index> {name});
    if (iter == m_index.end())
      throw std::runtime_error{
        red::eno::get_msg(red::eno::ERRNO_INDEX_ERROR)};
    return *iter;
  }

  /***************************************************************************
   * Get the schema's name.
   */
  std::string schema::name () const {
    return m_header.db_name() + '.' + m_header.tbl_name();
  }

  /***************************************************************************
   *
   */
  void schema::remove_index(std::string const& ndx_name) {
    // do not remove primary-key index
    if (ndx_name == red::schema::PK_NDX) return;
    // find index
    auto iter = std::find_if(m_index.begin(), m_index.end(),
        [&](auto& ndx){return(ndx.name() == ndx_name);});
    if (iter == m_index.end()) return;
    // remove index
    m_index.erase(iter);
    // update index count in header
    m_header.index_count(m_header.index_count() - 1);
  }

  /***************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef SCHEMA_DEBUG
  std::ostream& operator<<(std::ostream& os, schema& o) {
    os << "TABLE:\t" << o.name() << '\n';
    os << o.get_header() << '\n';
    for (auto col : o.get_column_list())
      os << col << '\n';
    for (auto ndx : o.get_index_list())
      os << ndx << '\n';
    for (auto fgn : o.get_foreign_list())
      os << fgn << '\n';
    return os;
  }
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS SCHEMA: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS MANAGER: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS MANAGER: Destructor
   */
  manager::~manager () {
    for (auto p : m_schema)
      save_schema(p.first);
  }

  /***************************************************************************
   * CLASS MANAGER: Copy Assignment Operator
   */
  manager& manager::operator=(manager const& o) {
    m_path = o.m_path;
    for (std::pair<std::string, std::shared_ptr<schema> > p: o.m_schema)
      m_schema [ p.first ] = p.second;
    return *this;
  }

  /***************************************************************************
   * CLASS MANAGER: Move Assignment Operator
   */
  manager& manager::operator=(manager&& o) {
    m_path = o.m_path;
    for (std::pair<std::string,
        std::shared_ptr<schema> > p: o.m_schema)
      m_schema [ p.first ] = p.second;
    return *this;
  }

  /***************************************************************************
   *
   */
  void manager::save_schema (std::string const& sch_name) {
    std::shared_ptr<schema> sch = m_schema [ sch_name ];
    if (sch != nullptr) save_schema(sch);
  }

  /***************************************************************************
   * Save a schema to disk. The name of the target schema is passed as a para-
   * meter. The schema is saved in JSON format.
   */
  void manager::save_schema (std::shared_ptr<red::schema::schema> sch) {
    JsonBuilder* builder = json_builder_new();

    json_builder_begin_object(builder);

    sch->m_header.to_json_object(builder);

    int32_t count {0};
    for (column col : sch->m_column)
      col.to_json_object(builder, count++);

    for (index ndx : sch->m_index)
      ndx.to_json_object(builder);

    for (foreign fgn : sch->m_foreign)
      fgn.to_json_object(builder);

    json_builder_end_object(builder);

    JsonGenerator* generator = json_generator_new();

    JsonNode* root = json_builder_get_root(builder);
    json_generator_set_root(generator, root);

    std::ostringstream sch_path;
    sch_path << m_path << '/' << sch->name() << ".json";

    GError* gerror = nullptr;
    if (!json_generator_to_file(generator, sch_path.str().c_str (), &gerror)) {
      g_error_free(gerror);
      throw std::runtime_error{
        red::eno::get_msg(red::eno::ERRNO_SAVE_SCH_ERROR)};
    }

    json_node_unref(root);
    g_object_unref(generator);
    g_object_unref(builder);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS MANAGER: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Load a schema from disk. On disk the schema is in JSON format.
   */
  std::shared_ptr<schema> manager::load_schema (std::string const& sch_name) {
    JsonParser* parser = json_parser_new();

    std::ostringstream sch_path;
    sch_path << m_path << '/' << sch_name << ".json";

    errno = 0;
    struct stat sb;
    if (stat(sch_path.str().c_str(), &sb) < 0) return nullptr;

    GError* error = nullptr;
    if (!json_parser_load_from_file(parser, sch_path.str().c_str(), &error)) {
      g_error_free(error);
      g_object_unref(parser);
      throw std::runtime_error{
        red::eno::get_msg(red::eno::ERRNO_LOAD_SCH_ERROR)};
    }

    header hdr;
    std::vector<std::pair<std::string, column> > col_vec;
    std::vector<std::pair<std::string, index> > ndx_vec;
    std::vector<std::pair<std::string, foreign> > fgn_vec;

    JsonNode* root = json_parser_get_root(parser);

    if (JSON_NODE_HOLDS_OBJECT(root)) {
      JsonObject* object = json_node_get_object(root);

      JsonObjectIter iter;
      json_object_iter_init(&iter, object);

      gchar const* name = nullptr;
      JsonNode* node;
      while (json_object_iter_next(&iter, &name, &node)) {
        if (strcmp(name, "header") == 0)
          hdr.from_json_object(node);
        else if (strncmp(name, "column", 6) == 0) {
          column col;
          col.from_json_object(node);
          col_vec.push_back(std::make_pair(name, col));
        } else if (strncmp(name, "NDX_", 4) == 0) {
          index ndx;
          ndx.from_json_object(node);
          ndx_vec.push_back(std::make_pair(name, ndx));
        } else if (strncmp(name, "FGN_", 4) == 0) {
          foreign fgn;
          fgn.from_json_object(node);
          fgn_vec.push_back(std::make_pair(name, fgn));
        }
      }
    }

//    json_node_unref (root);
    g_object_unref(parser);

    std::sort(col_vec.begin(), col_vec.end(),
        [](auto x, auto y){return(x.first < y.first);});

    std::sort(ndx_vec.begin(), ndx_vec.end(),
        [](auto x, auto y){return(x.first < y.first);});

    std::sort(fgn_vec.begin(), fgn_vec.end(),
        [](auto x, auto y){return(x.first < y.first);});

    std::vector<column> col_list;
    std::vector<index> ndx_list;
    std::vector<foreign> fgn_list;

    for (auto p : col_vec) col_list.push_back(p.second);
    for (auto p : ndx_vec) ndx_list.push_back(p.second);
    for (auto p : fgn_vec) fgn_list.push_back(p.second);

    std::shared_ptr<schema> sch {
      new schema {hdr, col_list, ndx_list, fgn_list}};

    m_schema [ sch_name ] = sch;

    return sch;
  }

} // namespace


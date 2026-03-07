//-----------------------------------------------------------------------------
// Desc: Virtual machine library
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

#ifndef VM_H
#define VM_H 1

#define GCRYPT_NO_MPI_MACROS 1
#define GCRYPT_NO_DEPRECATED 1

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <gcrypt.h>
#include <stdexcept>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <memory>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <random>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include "err.h"
#include "logger.h"
#include "util.h"
#include "comm.h"
#include "buffer.h"
#include "schema.h"
#include "lock.h"
#include "cache.h"
#include "btree.h"
#include "opcode.h"
#include "dbms.h"
#include "token.h"
#include "srvcry.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * STRUCT WORK_SCHEMA
   */
  class work_schema {
    public:
      work_schema () { }

      std::vector<schema::column>   m_column;
      std::vector<schema::index>    m_index;
      std::vector<schema::foreign>  m_foreign;
      std::vector<schema::reference> m_reference;
  };
  
  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS ITEM
   */
  class item {
    public:
      item ()
        : m_type {0}, m_action {0}, m_out_size {0}, m_out_prec {4},
        m_continue {false}, m_negate {false}, m_quote {false}
      { }

      explicit item (row& rw)
        : m_type {0}, m_action {0}, m_out_size {0}, m_out_prec {0},
        m_continue {false}, m_negate {false}, m_quote {false}, m_row {rw}
      { }

      std::shared_ptr<tree::tree> tree () const {
        return m_tree;
      }

      std::shared_ptr<schema::schema> schema () const {
        return tree()->get_schema();
      }

      int32_t col_type () const {
        return m_parent->schema()->get_column(m_col_name).type();
      }

      int32_t col_flag () const {
        return m_parent->schema()->get_column(m_col_name).flag();
      }

      int32_t col_size () const {
        return m_parent->schema()->get_column(m_col_name).size();
      }

      int32_t col_prec () const {
        return m_parent->schema()->get_column(m_col_name).prec();
      }
        
      void value (std::string const&);
      int32_t value () const  { return std::stoi(m_value); }

      int32_t       m_type;
      int32_t       m_action;
      int32_t       m_out_size; // output field size
      int32_t       m_out_prec; // output field precision
      bool          m_continue; // on failure continue or not
      bool          m_negate;
      bool          m_quote;
      std::string   m_value;
      std::string   m_title;
      std::string   m_alias;
      std::string   m_db_name;
      std::string   m_tbl_name;
      std::string   m_col_name;
      std::string   m_ndx_name;
      std::string   m_fgn_name;
      row           m_row;
      tree::iterator  m_niter;  // node iterator
      tree::iterator2 m_diter;  // data iterator
      std::shared_ptr<item> m_left;
      std::shared_ptr<item> m_right;
      std::shared_ptr<item> m_opt_prev; // optimized prev
      std::shared_ptr<item> m_prev;
      std::shared_ptr<item> m_next;
      std::shared_ptr<item> m_on_cond;
      std::shared_ptr<item> m_parent;
      std::shared_ptr<tree::tree>   m_tree;
  };

  /****************************************************************************
   *
   */
  void unlink_item (std::shared_ptr<item>);

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef VM_DEBUG
  std::ostream& operator<<(std::ostream&, std::shared_ptr<item>);
#endif

  int32_t const MAX_NOACK = 4;

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS VM
   */
  class vm {
      typedef std::chrono::high_resolution_clock Time;
      typedef std::chrono::milliseconds ms;
      typedef std::chrono::duration<float> fsec;
    public:
      vm (std::pair<buffer, buffer> buf,
          std::shared_ptr<dbms::dbms> a_dbms,
          std::shared_ptr<comm::fsm> a_fsm,
          std::shared_ptr<srvcry> a_crypto,
          std::shared_ptr<logger> a_logger)
        : m_have_inner_join {false},
        m_did_output {false},
        m_output_type {0},
        m_opcode {comm::OP_RSLT_FIN},
        m_tbl_index {-1L},
        m_data {buf.first},
        m_code {buf.second},
        m_y0 {Time::now()},
        m_dbms {a_dbms},
        m_fsm {a_fsm},
        m_crypto {a_crypto},
        m_logger{a_logger}
      {
        auto a_cb = std::bind(&vm::fsm_flush_cb, this);
        m_fsm->set_flush_cb(a_cb);
      }

      vm (int32_t opcode, std::pair<buffer, buffer> buf,
          std::shared_ptr<dbms::dbms> a_dbms,
          std::shared_ptr<comm::fsm> a_fsm,
          std::shared_ptr<srvcry> a_crypto,
          std::shared_ptr<logger> a_logger)
        : m_have_inner_join {false},
        m_did_output {false},
        m_output_type {0},
        m_opcode {opcode},
        m_tbl_index {-1L},
        m_data {buf.first},
        m_code {buf.second},
        m_y0 {Time::now()},
        m_dbms {a_dbms},
        m_fsm {a_fsm},
        m_crypto {a_crypto},
        m_logger{a_logger}
      {
        auto a_cb = std::bind(&vm::fsm_flush_cb, this);
        m_fsm->set_flush_cb(a_cb);
      }

      ~vm ();

      vm (vm const&) = delete;

      vm& operator=(vm const&) = delete;

      void run ();
    private:
      bool          m_have_inner_join;
      bool          m_did_output;
      int32_t       m_output_type;
      int32_t       m_opcode;
      ssize_t       m_tbl_index;
      buffer        m_data;
      buffer        m_code;
      std::string   m_outfile;
      std::vector<std::shared_ptr<item> > m_expr_list;
      std::vector<std::shared_ptr<item> > m_key_list;
      std::vector<std::shared_ptr<item> > m_work_list;
      std::vector<std::shared_ptr<item> > m_tree_list;
      std::stack<std::string> m_title_stack;
      std::deque<std::pair<std::string,
        std::shared_ptr<item> > > m_assignment_list;
      std::stack<std::shared_ptr<item> >  m_stack;
      std::multimap<std::string, std::string> m_output;
      std::chrono::time_point<std::chrono::high_resolution_clock> m_t0;
      std::chrono::time_point<std::chrono::high_resolution_clock> m_y0;
      std::shared_ptr<item> m_head;
      std::shared_ptr<item> m_tail;
      std::shared_ptr<item> m_where;
      std::shared_ptr<work_schema>  m_wrk_sch;  // working schema
      std::shared_ptr<schema::schema> m_schema;
      std::shared_ptr<dbms::dbms> m_dbms;
      std::shared_ptr<comm::fsm>    m_fsm;
      std::shared_ptr<srvcry> m_crypto;
      std::shared_ptr<logger> m_logger;

      void add ();
      void alter_database ();
      void add_column ();
      void add_foreign_key ();
      void add_index ();
      void alter_user ();
      void call ();
      void compact_add (std::shared_ptr<item>);
      void compact_and (std::shared_ptr<item>);
      void compact_cmpeq (std::shared_ptr<item>);
      void compact_cmpge (std::shared_ptr<item>);
      void compact_cmpgt (std::shared_ptr<item>);
      void compact_cmple (std::shared_ptr<item>);
      void compact_cmplt (std::shared_ptr<item>);
      void compact_cmpne (std::shared_ptr<item>);
      void compact_condition (std::shared_ptr<item>);
      void compact_div (std::shared_ptr<item>);
      void compact_mod (std::shared_ptr<item>);
      void compact_mul (std::shared_ptr<item>);
      void compact_or (std::shared_ptr<item>);
      void compact_sub (std::shared_ptr<item>);
      void compact_xor (std::shared_ptr<item>);
      void cmpeq ();
      void cmpge ();
      void cmpgt ();
      void cmple ();
      void cmplt ();
      void cmpne ();
      void create_database ();
      void create_table ();
      void create_user ();
      void cross_join ();
      void delete_row ();
      void describe_column ();
      void describe_key ();
      void describe_table ();
      void div ();
      void drop_column ();
      void drop_database ();
      void drop_foreign_key ();
      void drop_index ();
      void drop_table ();
      void drop_user ();
      void drop_user_global (std::string const&, std::string const&);
      void drop_user_local (std::string const&, std::string const&);
      void exec_add (std::string&, std::shared_ptr<item>);
      void exec_and (std::string&, std::shared_ptr<item>);
      void exec_assignment (std::string&, std::shared_ptr<item>);
      void exec_cmpeq (std::string&, std::shared_ptr<item>);
      void exec_cmpge (std::string&, std::shared_ptr<item>);
      void exec_cmpgt (std::string&, std::shared_ptr<item>);
      void exec_cmple (std::string&, std::shared_ptr<item>);
      void exec_cmplt (std::string&, std::shared_ptr<item>);
      void exec_cmpne (std::string&, std::shared_ptr<item>);
      void exec_condition (std::string&, std::shared_ptr<item>);
      void exec_div (std::string&, std::shared_ptr<item>);
      void exec_inner_join (std::shared_ptr<item>);
      void exec_inner_join_x_0 (std::shared_ptr<item>);
      void exec_inner_join_x_1 (std::shared_ptr<item>);
      void exec_inner_join_0_0 (std::shared_ptr<item>);
      void exec_inner_join_0_1 (std::shared_ptr<item>);
      void exec_join (std::shared_ptr<item>);
      void exec_left_join (std::shared_ptr<item>);
      void exec_left_join_x_0 (std::shared_ptr<item>);
      void exec_left_join_x_1 (std::shared_ptr<item>);
      void exec_left_join_0_0 (std::shared_ptr<item>);
      void exec_left_join_0_1 (std::shared_ptr<item>);
      void exec_mod (std::string&, std::shared_ptr<item>);
      void exec_mul (std::string&, std::shared_ptr<item>);
      void exec_or (std::string&, std::shared_ptr<item>);
      void exec_query ();
      void exec_sub (std::string&, std::shared_ptr<item>);
      void exec_where (std::shared_ptr<item>);
      void exec_xor (std::string&, std::shared_ptr<item>);
      void fsm_flush ();
      void fsm_flush_cb () { m_fsm->cont(); m_fsm->loop(m_fsm); }
      void get_column_name (std::vector<std::pair<std::string, std::string> >&,
          std::shared_ptr<schema::schema>);
      bool get_data (std::vector<std::pair<std::string, std::string> >&,
          std::string const&);
      void get_index_name (std::shared_ptr<item>,
          std::shared_ptr<item>, std::shared_ptr<item>);
      std::string get_varchar (int32_t);
      void grant ();
      void grant_global (std::string const&, std::string const&,
          std::vector<std::string>&);
      void grant_local (std::string const&, std::string const&,
          std::string const&, std::string const&, std::vector<std::string>&);
      void inner_join ();
      void insert ();
      bool is_number (float&, std::string const&);
      void left_join ();
      void load_column_name (std::shared_ptr<schema::schema>,
          std::vector<std::pair<std::string, std::string> >&);
      void load_data ();
      void load_foreign_tree (
          std::vector<std::pair<std::string, std::shared_ptr<tree::tree> > >&,
          std::vector<schema::foreign> const&);
      void load_row (row&,
          std::vector<std::pair<std::string, std::string> > const&);
      void load_value_list (std::vector<std::pair<std::string, std::string> >&);
      void logic_and ();
      void logic_not ();
      void logic_or ();
      void make_assignment ();
      void make_asterisk_expression ();
      void make_column_definition ();
      void make_column_expression ();
      void make_column_list (std::vector<std::pair<std::shared_ptr<item>,
          std::shared_ptr<item> > >&, std::shared_ptr<item>,
          std::shared_ptr<item>, std::shared_ptr<item>);
      void make_enum_list (std::vector<std::string>&, int32_t&);
      void make_expression ();
      void make_expression_header ();
      void make_expression_list ();
      void make_foreign_key ();
      void make_primary_key ();
      void make_query ();
      void make_reference (std::string const&, std::string const&,
          std::vector<schema::foreign> const&);
      void make_work_column_list (std::vector<schema::column>&,
          std::shared_ptr<item>, std::shared_ptr<item>);
      void make_unique_key ();
//      void make_update_key (std::string&, std::shared_ptr<schema::schema>,
//          std::vector<std::pair<std::string, std::string> >&);
      void make_user ();
      void make_value_list (std::vector<std::pair<std::string, std::string> >&,
          std::shared_ptr<tree::tree>,
          std::vector<std::pair<std::string, std::string> >&);
      void make_working_schema ();
      void mul ();
      void negate ();
      void on_cond ();
//      void open_schema ();
      void open_table ();
      void order_by ();
      void output_boxed_line (std::string&);
      size_t output_boxed_result ();
      void output_expr_list ();
      size_t output_result ();
      size_t output_query_result ();
//      void parse_where (std::shared_ptr<item>,
//          std::vector<std::pair<std::string, std::string> >&);
      void push ();
      void pusho ();
      void revoke ();
      void revoke_global (std::string const&, std::string const&,
          std::vector<std::string>&);
      void revoke_local (std::string const&, std::string const&,
          std::string const&, std::string const&, std::vector<std::string>&);
      void ret () { m_fsm->set_flush_cb(nullptr); m_fsm->cont(); }
      void right_join ();
      void select ();
      void set_column_value (std::shared_ptr<item>, row&);
      void set_column_value (std::shared_ptr<item>, row&, row&);
      void set_column_value_null (std::shared_ptr<item>);
      void set_outfile ();
      void shared_lock_tree () {
        m_tree_list.back()->tree()->set_shared_lock();
      }
      void show_databases ();
      void show_tables ();
      void sub ();
      schema::foreign to_foreign_key (schema::column,
          std::string const&, std::string const&, std::string const&);
      schema::foreign to_foreign_key (std::vector<schema::column>&,
          std::string const&, std::string const&, std::vector<std::string>&);
      void unique_lock_tree () {
        m_tree_list.back()->tree()->set_unique_lock();
      }
      void update ();
      void use_database ();
      void using_cond ();
      bool verify_foreign_key (std::vector<schema::foreign> const&,
          std::vector<std::pair<std::string,
            std::shared_ptr<tree::tree> > > const&, row&);
      bool verify_global_privilege (std::string const&);
      bool verify_local_privilege (std::string const&, std::string const&,
          std::string const&);
      void verify_privilege (std::string const&, std::string const&,
          std::string const&);
      void verify_privilege (std::string const&, std::string const&,
          std::vector<std::string> const&);
      bool verify_unique_key (std::shared_ptr<tree::tree>, row&);
      void verify_value_list (std::shared_ptr<tree::tree>,
          std::vector<std::pair<std::string, std::string> >&);
      void where ();
      void yield ();
  };

} // namespace

#endif  // VM_H


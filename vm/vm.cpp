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

#include "vm.h"

namespace red {

  /****************************************************************************
   * VM Types
   */
  int32_t const TYPE_NAME     = 16;

  int32_t const TYPE_NULL     = 0;
  int32_t const TYPE_ADD      = 100;
  int32_t const TYPE_ASSIGN   = 101;
  int32_t const TYPE_BOOLEAN  = 102;
  int32_t const TYPE_BW_AND   = 103;
  int32_t const TYPE_BW_OR    = 104;
  int32_t const TYPE_BW_XOR   = 105;
  int32_t const TYPE_COLUMN   = 106;
  int32_t const TYPE_CMPEQ    = 107;
  int32_t const TYPE_CMPGE    = 108;
  int32_t const TYPE_CMPGT    = 109;
  int32_t const TYPE_CMPLE    = 110;
  int32_t const TYPE_CMPLT    = 111;
  int32_t const TYPE_CMPNE    = 112;
  int32_t const TYPE_CRJOIN   = 113;
  int32_t const TYPE_DIV      = 114;
  int32_t const TYPE_FLOAT    = 115;
  int32_t const TYPE_INJOIN   = 116;
  int32_t const TYPE_INT      = 117;
  int32_t const TYPE_AND      = 118;
  int32_t const TYPE_NOT      = 119;
  int32_t const TYPE_OR       = 120;
  int32_t const TYPE_LTJOIN   = 121;
  int32_t const TYPE_MOD      = 122;
  int32_t const TYPE_MUL      = 123;
  int32_t const TYPE_OFFSET   = 124;
  int32_t const TYPE_ON       = 125;
  int32_t const TYPE_OUTPUT   = 126;
  int32_t const TYPE_ORDERBY  = 127;
  int32_t const TYPE_RTJOIN   = 128;
  int32_t const TYPE_SCHEMA_T = 129;
  int32_t const TYPE_SUB      = 130;
  int32_t const TYPE_TREE     = 131;
  int32_t const TYPE_USING    = 132;
  int32_t const TYPE_VARCHAR  = 133;
  int32_t const TYPE_WHERE    = 134;
  int32_t const TYPE_XOR      = 135;

  int32_t const TYPE_STD_OUTPUT   = 0;
  int32_t const TYPE_FILE_OUTPUT  = 1;

  char const COMMA {','};
  char const DOT {'.'};
  char const MS {'-'};
  char const NL {'\n'};
  char const PS {'+'};
  char const SPC {' '};
  char const TAB {'\t'};
  char const VB {'|'};
  char FS {'\t'};

  std::string const AUTO_INCREMENT {"auto_increment"};

  std::string const VM_HDR {"VIRTUAL MACHINE: "};

  std::string const NULL_VALUE;

  std::string const VM_FALSE {"false"};
  std::string const VM_TRUE {"true"};

  constexpr int32_t MIN_RAND_VALUE {0};
  constexpr int32_t MAX_RAND_VALUE {std::numeric_limits<int32_t>::max()};

  std::vector<std::string> rand_list {
    {"GX0KQ9ya"},
      {"Z1usOp2T"},
      {"wh3HW8Az"},
      {"oE4nRekP"},
      {"SYmc5I7D"},
      {"VjlN6LgU"},
      {"bYr9JqxB"},
      {"CvFidtK"}
  };

  size_t const FLD_FIELD  = 0L;
  size_t const FLD_TYPE   = 1L;
  size_t const FLD_NULL   = 2L;
  size_t const FLD_PRIKEY = 3L;
  size_t const FLD_UNIKEY = 4L;
  size_t const FLD_MULKEY = 5L;
  size_t const FLD_EXTRA  = 6L;

  size_t const FLD_KEY    = 0L;
//size_t const FLD_TYPE   = 1L;
  size_t const FLD_COL    = 2L;
  size_t const FLD_TBL    = 3L;

  std::vector<std::pair<std::string, size_t> > const COL_META_LIST {
    {"Field", 5}, {"Type", 4}, {"Null", 4}, {"PRI", 6}, {"UNI", 6},
    {"MUL", 6}, {"Extra", 5}
  };

  std::vector<std::pair<std::string, size_t> > const NDX_META_LIST {
    {"Key", 3}, {"Type", 4}, {"Column", 6}, {"Table", 5}
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS DATA_LEXER
   */
  class data_lexer {
      static char const EOI  = static_cast<char>(255);
      int32_t const NUMBER  = 1;
      int32_t const VARCHAR = 2;
    public:
      data_lexer (std::string const& input)
        : m_ch {'\0'}, m_index {0L}, m_input {input}
      {
        if (m_input.size()) m_ch = m_input [ m_index ];
        else m_ch = EOI;
      }

      token next_token ();
    private:
      char  m_ch;
      size_t  m_index;
      std::string m_input;

      void consume ();
      token number ();
      token varchar ();
      void ws ();

      friend class vm;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS DATA_LEXER: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  token data_lexer::next_token () {
    while (m_ch != data_lexer::EOI) {
      switch (m_ch) {
        case ' ': case '\t': ws(); break;
        case '\"': case '\'': return varchar();
        case ',': consume(); break;
        case '.': return number();
        default: return number();
      }
    }
    return token{static_cast<int32_t>(data_lexer::EOI), "<EOI>"};
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS DATA_LEXER: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void data_lexer::consume () {
    if (m_index + 1 < m_input.size()) {
      m_ch = m_input[ ++m_index ];
    } else m_ch = data_lexer::EOI;
  }

  /****************************************************************************
   *
   */
  token data_lexer::number () {
    std::string value;
    while (std::isdigit(m_ch)) { value += m_ch; consume(); }
    if (m_ch == '.') {
      value += m_ch;
      consume();
      while (std::isdigit(m_ch)) { value += m_ch; consume(); }
    }
    return token {data_lexer::NUMBER, value};
  }

  /****************************************************************************
   *
   */
  token data_lexer::varchar () {
    std::string value{};
    char delim {m_ch};
    consume();  // consume first '\"' || '\''
    while (m_ch != '\"' && m_ch != '\'' && m_ch != data_lexer::EOI) {
      value += m_ch;
      consume();
    }
    if (m_ch != delim)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_TOKEN_ERROR)};
    consume();  // consume last '\"' || '\''
    return token {data_lexer::VARCHAR, value};
  }

  /****************************************************************************
   *
   */
  void data_lexer::ws () {
    while (std::isspace(m_ch)) consume();
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS WORK_SCHEMA: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS ITEM: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void unlink_item (std::shared_ptr<item> it) {
    if (it == nullptr) return;
    unlink_item(it->m_left);
    unlink_item(it->m_right);
    unlink_item(it->m_on_cond);
    it->m_left = nullptr;
    it->m_right = nullptr;
    it->m_opt_prev = nullptr;
    it->m_prev = nullptr;
    it->m_next = nullptr;
    it->m_on_cond = nullptr;
    it->m_parent = nullptr;
  }

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef VM_DEBUG
  std::ostream& operator<<(std::ostream& os, std::shared_ptr<item> o) {
    os << "\ntype:\t" << o->m_type
      << "\naction:\t" << o->m_action;
    if (o->m_type == 106) {
      os << "\ncol type:\t" << o->col_type()
        << "\ncol flag:\t" << std::hex << std::setw(2) << std::setfill('0')
        << o->col_flag() << std::dec
        << "\ncol size:\t" << o->col_size()
        << "\ncol prec:\t" << o->col_prec();
    }
    os << "\nout size:\t" << o->m_out_size
      << "\nout prec:\t" << o->m_out_prec
      << "\ncontinue:\t" << std::boolalpha << o->m_continue
      << "\nvalue:\t" << o->m_value
      << "\ntitle:\t" << o->m_title
      << "\nalias:\t" << o->m_alias
      << "\ndb_name:\t" << o->m_db_name
      << "\ntbl_name:\t" << o->m_tbl_name
      << "\ncol_name:\t" << o->m_col_name
      << "\nndx_name:\t" << o->m_ndx_name
      << "\nfgn_name:\t" << o->m_fgn_name;
    if (o->m_prev != nullptr) os << "\nprev:\t" << o->m_prev->m_tbl_name;
    else os << "\nprev:\tnullptr";
    if (o->m_next != nullptr) os << "\nnext:\t" << o->m_next->m_tbl_name;
    else os << "\nnext:\tnullptr";
    if (o->m_on_cond != nullptr)
      os << "\non cond:\t" << (void*)o->m_on_cond.get();
    else os << "\non cond:\tnullptr";
    return os;
  }
#endif

  /****************************************************************************
   *
   */
  void item::value (std::string const& value) {
    if (m_negate) {
      switch (m_type) {
        case red::TYPE_COLUMN:
          switch (col_type()) {
            case schema::TYPE_FLOAT:
              {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << -std::stof(value);
                m_value = oss.str();
              }
              break;
            case schema::TYPE_INT:
              m_value = std::to_string(-std::stoi(value));
              break;
            default:;
          }
          break;
        case red::TYPE_FLOAT:
          {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << -std::stof(value);
            m_value = oss.str();
          }
          break;
        case red::TYPE_INT:
          m_value = std::to_string(-std::stoi(value));
          break;
        default:;
      }
    } else m_value = value;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS VM: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS VM: Destructor
   */
  vm::~vm () {
    auto curr = m_head;
    while (curr) {
      auto next = curr->m_next;
      curr->m_opt_prev = nullptr;
      curr->m_prev = nullptr;
      curr->m_next = nullptr;
      curr = next;
    }
    m_head  = nullptr;
    m_tail  = nullptr;
    for (auto it : m_expr_list) unlink_item(it);
    m_expr_list.clear();
    for (auto it : m_key_list) unlink_item(it);
    m_key_list.clear();
    for (auto it : m_work_list) unlink_item(it);
    m_work_list.clear();
    for (auto it : m_tree_list) unlink_item(it);
    m_tree_list.clear();
    for (auto p : m_assignment_list) unlink_item(p.second);
    m_assignment_list.clear();
    while (not m_stack.empty()) {
      unlink_item(m_stack.top());
      m_stack.pop();
    }
    m_output.clear();
    unlink_item(m_where);
    m_where  = nullptr;
  }

  /****************************************************************************
   * Execute SQL statement
   */
  void vm::run () {
    m_t0 = Time::now();

    while (true) {
      switch (m_code.get()) {
        case OP_CALL: call(); break;
        case OP_PUSH: push(); break;
        case OP_PUSHO: pusho(); break;
        case OP_RET: ret(); return;
        default: throw std::runtime_error{eno::get_msg(
                     eno::ERRNO_OPCODE_ERROR)};
      }
    }
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS VM: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Construct an addition (+) item and push onto stack.
   */
  void vm::add () {
    item* it = new item;
    it->m_type  = TYPE_ADD;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   *
   */
  void vm::add_column () {
    try {
      // pop item off stack
      auto e1 = m_stack.top(); m_stack.pop();
      // get tree
      auto the_tree = m_tree_list.back()->tree();
      // verify privilege
      verify_privilege(the_tree->get_schema()->get_header().db_name(),
          the_tree->get_schema()->get_header().tbl_name(), dbms::ALTER_PRIV);
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << ">>> ALTER TABLE <name> ADD COLUMN is not currently supported.";
      oss << " [Target Version: 2.0]\n";
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::add_foreign_key () {
    try {
      // get foreign tree
      auto fgn_tree = m_tree_list.front()->tree();
      // verify privilege
      verify_privilege(fgn_tree->db_name(), fgn_tree->tbl_name(),
          dbms::ALTER_PRIV);
      // get the tree
      auto the_tree = m_tree_list.back()->tree();
      // verify privilege
      verify_privilege(the_tree->db_name(), the_tree->tbl_name(),
          dbms::ALTER_PRIV);
      // pop item off stack
      auto e1 = m_stack.top(); m_stack.pop();
      // get count
      int32_t count = e1->value();
      // process column names
      std::vector<std::string> col_name_list;
      while (count-- > 0) {
        // pop item off stack
        auto e2 = m_stack.top(); m_stack.pop();
        // get privilege
        int32_t offset2 = e2->value();
        // push privilege onto list
        col_name_list.push_back(get_varchar(offset2 + 1));
      }
      // verify names in column-name-list correspond to names of actual columns
      // in the schema
      auto& column_list = the_tree->get_schema()->get_column_list();
      for (auto col_name : col_name_list) {
        auto iter = std::find_if(column_list.begin(), column_list.end(),
            [&](auto& col){return(col.name() == col_name);});
        if (iter == column_list.end())
          throw std::runtime_error{
            eno::get_msg(eno::ERRNO_MALFORMED_COLUMN_LIST)};
      }
      // pop item off stack
      e1 = m_stack.top(); m_stack.pop();
      // get count
      count = e1->value();
      // process column names
      std::vector<std::string> fgn_name_list;
      while (count-- > 0) {
        // pop item off stack
        auto e2 = m_stack.top(); m_stack.pop();
        // get privilege
        int32_t offset2 = e2->value();
        // push privilege onto list
        fgn_name_list.push_back(get_varchar(offset2 + 1));
      }
      // verify names in foreign column-name-list correspond to names of actual
      // columns in the primary-key of the foreign schema
      auto& fgn_ndx = fgn_tree->get_schema()->get_index(schema::PK_NDX);
      for (auto col_name : fgn_name_list) {
        auto iter = std::find_if(fgn_ndx.col_name_list().begin(),
            fgn_ndx.col_name_list().end(),
            [&](auto& name){return(name == col_name);});
        if (iter == fgn_ndx.col_name_list().end())
          throw std::runtime_error{
            eno::get_msg(eno::ERRNO_MALFORMED_COLUMN_LIST)};
      }
      // make foreign-key name
      auto fgn_name = schema::name<schema::foreign>{schema::FGN_,
        the_tree->get_schema()->get_foreign_list()}.make();
      // emplace foreign-key in list
      the_tree->get_schema()->get_foreign_list().emplace_back(fgn_name,
          fgn_tree->db_name(), fgn_tree->tbl_name(), col_name_list);
      // make reference name and emplace reference for each foreign column
      for (auto col_name : fgn_name_list) {
        auto& col = fgn_tree->get_schema()->get_column(col_name);
        auto ref_name = schema::name<schema::reference>{schema::REF_,
          col.reference_list()}.make();
        col.reference_list().emplace_back(ref_name, the_tree->db_name(),
            the_tree->tbl_name(), fgn_name);
      }
      // save updated schema
      m_dbms->get_schema_mgr()->save_schema(the_tree->name());
      // save updated schema
      m_dbms->get_schema_mgr()->save_schema(fgn_tree->name());
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::add_index () {
    try {
      // get tree
      auto the_tree = m_tree_list.back()->tree();
      // verify privilege
      verify_privilege(the_tree->get_schema()->get_header().db_name(),
          the_tree->get_schema()->get_header().tbl_name(), dbms::ALTER_PRIV);
      // pop item off stack
      auto e1 = m_stack.top(); m_stack.pop();
      // get count
      int32_t count = e1->value();
      // process column names
      std::vector<std::string> col_name_list;
      while (count-- > 0) {
        // pop item off stack
        auto e2 = m_stack.top(); m_stack.pop();
        // get privilege
        int32_t offset2 = e2->value();
        // push privilege onto list
        col_name_list.push_back(get_varchar(offset2 + 1));
      }
      // construct column/index flag
      int32_t flag {(schema::FLAG_NOT_NULL | schema::FLAG_MULTIPLE)};
      // verify that names in the col_name_list correspond to actual column
      // names in the schema and calculate key size and flag columns
      int32_t key_size {0};
      for (auto col_name : col_name_list) {
        auto iter = std::find_if(the_tree->get_schema()->get_column_list()
            .begin(), the_tree->get_schema()->get_column_list().end(),
            [&](auto& col){return(col.name() == col_name);});
        if (iter == the_tree->get_schema()->get_column_list().end())
          throw std::runtime_error{
            eno::get_msg(eno::ERRNO_MALFORMED_COLUMN_LIST)};
        key_size += (*iter).size();
        (*iter).flag((*iter).flag() | flag);
      }
      // construct binary tree calculator
      tree::bcalc bc {key_size};
      // make name of new index
      std::string ndx_name = schema::name<schema::index>{schema::NDX_,
        the_tree->get_schema()->get_index_list()}.make();
      // construct new index
      schema::index ndx {flag, bc.order(), key_size, ndx_name, col_name_list};
      // add index to schema
      the_tree->get_schema()->get_index_list().push_back(ndx);
      // update index-count in schema
      the_tree->get_schema()->get_header()
        .index_count(the_tree->get_schema()->get_header().index_count() + 1);
      // construct fill-index object and fill index
      tree::fill_index{ndx_name, the_tree}.fill();
      // flush the tree
      m_dbms->flush(the_tree);
      // save updated schema
      m_dbms->get_schema_mgr()->save_schema(the_tree->name());
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::alter_database () {
  }

  /****************************************************************************
   *
   */
  void vm::alter_user () {
    try {
      // lock mutex to prevent race condition
      // TODO: m_dbms->unique_lock_userd_mutex();
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get username
      std::string username = get_varchar(offset + 1);
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset
      offset = e->value();
      // get hostname
      std::string hostname = get_varchar(offset + 1);
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset
      offset = e->value();
      // get new-password
      std::string new_password = get_varchar(offset + 1);
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset
      offset = e->value();
      // get old-password
      std::string old_password = get_varchar(offset + 1);
      // verify privilege
      verify_privilege(dbms::ASTERISK, dbms::ASTERISK,
          dbms::CREATE_USER_PRIV);
      // construct block-cache name (schema-name)
      std::ostringstream bc_name;
      bc_name << dbms::ETC_DIR << '.' << dbms::CRED_TBL;
      // get block-cache
      auto blk_cache = m_dbms->get_block_cache(bc_name.str());
      // if the block-cache was not found throw exceptoin
      if (blk_cache == nullptr)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_TABLE_REF)};
      // construct tree object
      auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
      // unique-lock tree
      the_tree->set_unique_lock();
      // construct row
      auto org_row = the_tree->make_row();
      // populate row
      org_row.append(0, hostname);
      org_row.append(1, username);
      // construct key
      std::string key = org_row.make_key(schema::PK_NDX);
      // does user exist
      auto iter = the_tree->find(schema::PK_NDX, key);
      if (iter == the_tree->end(schema::PK_NDX))
        throw std::runtime_error{eno::get_msg(eno::ERRNO_MALFORMED_NAME)};
      // get original row
      org_row = *iter;
      if (not old_password.empty()) {
        // 2020-01-24: fix to prevent plain text password from being prepended
        // to hashed password
        std::string hashed_old_password;
        // hash old-password
        if (not m_crypto->hash_text(hashed_old_password, old_password))
          throw std::runtime_error{eno::get_msg(eno::ERRNO_CRYPTO_ERROR)};
        // verify old-password is current-password
        if (hashed_old_password != org_row.value_at(dbms::PWRD_FLD))
          throw std::runtime_error{eno::get_msg(eno::ERRNO_BAD_PASSWORD)};
      }
      // 2020-01-24: fix to prevent plain text password from being prepended
      // to hashed password
      std::string hashed_new_password;
      // hash new-password
      if (not m_crypto->hash_text(hashed_new_password, new_password))
        throw std::runtime_error{eno::get_msg(eno::ERRNO_CRYPTO_ERROR)};
      // construct update-row
      auto upd_row = the_tree->make_row();
      // populate update-row
      upd_row.init(org_row);
      upd_row.append(0, org_row.value_at(0));
      upd_row.append(1, org_row.value_at(1));
      upd_row.append(2, hashed_new_password);
      for (size_t i = 3; i < static_cast<size_t>(upd_row
            .get_schema()->get_header().column_count()); ++i)
      {
        upd_row.append(i, org_row.value_at(i));
      }
      // construct emplace updater
      auto emdate = tree::emplace{the_tree};
      // do update
      emdate.update(upd_row);
      // flush tree
      m_dbms->flush(the_tree);
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   * Execute function call
   */
  void vm::call () {
    switch (m_code.get()) {
      case OP_ADD: add(); break;
      case OP_ADDCLMN: add_column(); break;
      case OP_ADDFGNKEY: add_foreign_key(); break;
      case OP_ADDNDX: add_index(); break;
      case OP_ALTDB: alter_database(); break;
      case OP_ALTUSR: alter_user(); break;
      case OP_AND: logic_and(); break;
      case OP_CMPEQ: cmpeq(); break;
      case OP_CMPGE: cmpge(); break;
      case OP_CMPGT: cmpgt(); break;
      case OP_CMPLE: cmple(); break;
      case OP_CMPLT: cmplt(); break;
      case OP_CMPNE: cmpne(); break;
      case OP_CRJOIN: cross_join(); break;
      case OP_CRTDB: create_database(); break;
      case OP_CRTTBL: create_table(); break;
      case OP_CRTUSR: create_user(); break;
      case OP_DELETE: delete_row(); break;
      case OP_DESCRIBE: describe_table(); break;
      case OP_DIV: div(); break;
      case OP_DROPCLMN: drop_column(); break;
      case OP_DROPDB: drop_database(); break;
      case OP_DROPFGNKEY: drop_foreign_key(); break;
      case OP_DROPNDX: drop_index(); break;
      case OP_DROPTBL: drop_table(); break;
      case OP_DROPUSR: drop_user(); break;
      case OP_GRANT: grant(); break;
      case OP_INJOIN: inner_join(); break;
      case OP_INSERT: insert(); break;
      case OP_LOADDATA: load_data(); break;
      case OP_LTJOIN: left_join(); break;
      case OP_MKASSIGN: make_assignment(); break;
      case OP_MKASTEXPR: make_asterisk_expression(); break;
      case OP_MKCLMNDEF: make_column_definition(); break;
      case OP_MKCLMNEXPR: make_column_expression(); break;
      case OP_MKEXPR: make_expression(); break;
      case OP_MKEXPRHDR: make_expression_header(); break;
      case OP_MKEXPRLST: make_expression_list(); break;
      case OP_MKFGNKEY: make_foreign_key(); break;
      case OP_MKPRIKEY: make_primary_key(); break;
      case OP_MKUNIKEY: make_unique_key(); break;
      case OP_MKUSR: make_user(); break;
      case OP_MKWRKSCH: make_working_schema(); break;
      case OP_MUL: mul(); break;
      case OP_NEGATE: negate(); break;
      case OP_NOT: logic_not(); break;
      case OP_ON: on_cond(); break;
      case OP_OPNTBL: open_table(); break;
      case OP_OR: logic_or(); break;
      case OP_ORDERBY: order_by(); break;
      case OP_REVOKE: revoke(); break;
      case OP_RTJOIN: right_join(); break;
      case OP_SELECT: select(); break;
      case OP_SETOUTF: set_outfile(); break;
      case OP_SHOWDBS: show_databases(); break;
      case OP_SHOWTBLS: show_tables(); break;
      case OP_SLCKTBL: shared_lock_tree(); break;
      case OP_SUB: sub(); break;
      case OP_ULCKTBL: unique_lock_tree(); break;
      case OP_UPDATE: update(); break;
      case OP_USE: use_database(); break;
      case OP_USING: using_cond(); break;
      case OP_WHERE: where(); break;
      default:
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_OPCODE_ERROR)};
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_add (std::shared_ptr<item> on_cond) {
    if ((on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_FLOAT)
        || (on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_INT)
        || (on_cond->m_left->m_type == TYPE_INT
          && on_cond->m_right->m_type == TYPE_FLOAT))
    {
      float l_value = std::stof(on_cond->m_left->m_value);
      float r_value = std::stof(on_cond->m_right->m_value);

      float value = l_value + r_value;

      on_cond->m_type = TYPE_FLOAT;
      on_cond->m_value = std::to_string(value);
    } else if (on_cond->m_left->m_type == TYPE_INT
        && on_cond->m_right->m_type == TYPE_INT)
    {
      int32_t l_value = std::stoi(on_cond->m_left->m_value);
      int32_t r_value = std::stoi(on_cond->m_right->m_value);

      int32_t value = l_value + r_value;

      on_cond->m_type = TYPE_INT;
      on_cond->m_value = std::to_string(value);
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_and (std::shared_ptr<item> on_cond) {
    if (on_cond->m_left->m_type == TYPE_BOOLEAN
        && on_cond->m_right->m_type == TYPE_BOOLEAN)
    {
      bool l_value;
      bool r_value;
      std::istringstream(on_cond->m_left->m_value) >> std::boolalpha
        >> l_value;
      std::istringstream(on_cond->m_right->m_value) >> std::boolalpha
        >> r_value;
      std::ostringstream oss;
      oss << std::boolalpha << (l_value && r_value);
      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_cmpeq (std::shared_ptr<item> on_cond) {
    if ((on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_FLOAT)
        || (on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_INT)
        || (on_cond->m_left->m_type == TYPE_INT
          && on_cond->m_right->m_type == TYPE_FLOAT))
    {
      float l_value = std::stof(on_cond->m_left->m_value);
      float r_value = std::stof(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value == r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_INT
        && on_cond->m_right->m_type == TYPE_INT)
    {
      int32_t l_value = std::stoi(on_cond->m_left->m_value);
      int32_t r_value = std::stoi(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value == r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_VARCHAR
        && on_cond->m_right->m_type == TYPE_VARCHAR)
    {
      std::ostringstream oss;
      oss << std::boolalpha
        << (on_cond->m_left->m_value == on_cond->m_right->m_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_cmpge (std::shared_ptr<item> on_cond) {
    if ((on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_FLOAT)
        || (on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_INT)
        || (on_cond->m_left->m_type == TYPE_INT
          && on_cond->m_right->m_type == TYPE_FLOAT))
    {
      float l_value = std::stof(on_cond->m_left->m_value);
      float r_value = std::stof(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value >= r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_INT
        && on_cond->m_right->m_type == TYPE_INT)
    {
      int32_t l_value = std::stoi(on_cond->m_left->m_value);
      int32_t r_value = std::stoi(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value >= r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_VARCHAR
        && on_cond->m_right->m_type == TYPE_VARCHAR)
    {
      std::ostringstream oss;
      oss << std::boolalpha
        << (on_cond->m_left->m_value >= on_cond->m_right->m_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_cmpgt (std::shared_ptr<item> on_cond) {
    if ((on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_FLOAT)
        || (on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_INT)
        || (on_cond->m_left->m_type == TYPE_INT
          && on_cond->m_right->m_type == TYPE_FLOAT))
    {
      float l_value = std::stof(on_cond->m_left->m_value);
      float r_value = std::stof(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value > r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_INT
        && on_cond->m_right->m_type == TYPE_INT)
    {
      int32_t l_value = std::stoi(on_cond->m_left->m_value);
      int32_t r_value = std::stoi(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value > r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_VARCHAR
        && on_cond->m_right->m_type == TYPE_VARCHAR)
    {
      std::ostringstream oss;
      oss << std::boolalpha
        << (on_cond->m_left->m_value > on_cond->m_right->m_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_cmple (std::shared_ptr<item> on_cond) {
    if ((on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_FLOAT)
        || (on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_INT)
        || (on_cond->m_left->m_type == TYPE_INT
          && on_cond->m_right->m_type == TYPE_FLOAT))
    {
      float l_value = std::stof(on_cond->m_left->m_value);
      float r_value = std::stof(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value <= r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_INT
        && on_cond->m_right->m_type == TYPE_INT)
    {
      int32_t l_value = std::stoi(on_cond->m_left->m_value);
      int32_t r_value = std::stoi(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value <= r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_VARCHAR
        && on_cond->m_right->m_type == TYPE_VARCHAR)
    {
      std::ostringstream oss;
      oss << std::boolalpha
        << (on_cond->m_left->m_value <= on_cond->m_right->m_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_cmplt (std::shared_ptr<item> on_cond) {
    if ((on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_FLOAT)
        || (on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_INT)
        || (on_cond->m_left->m_type == TYPE_INT
          && on_cond->m_right->m_type == TYPE_FLOAT))
    {
      float l_value = std::stof(on_cond->m_left->m_value);
      float r_value = std::stof(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value < r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_INT
        && on_cond->m_right->m_type == TYPE_INT)
    {
      int32_t l_value = std::stoi(on_cond->m_left->m_value);
      int32_t r_value = std::stoi(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value < r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_VARCHAR
        && on_cond->m_right->m_type == TYPE_VARCHAR)
    {
      std::ostringstream oss;
      oss << std::boolalpha
        << (on_cond->m_left->m_value < on_cond->m_right->m_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_cmpne (std::shared_ptr<item> on_cond) {
    if ((on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_FLOAT)
        || (on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_INT)
        || (on_cond->m_left->m_type == TYPE_INT
          && on_cond->m_right->m_type == TYPE_FLOAT))
    {
      float l_value = std::stof(on_cond->m_left->m_value);
      float r_value = std::stof(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value != r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_INT
        && on_cond->m_right->m_type == TYPE_INT)
    {
      int32_t l_value = std::stoi(on_cond->m_left->m_value);
      int32_t r_value = std::stoi(on_cond->m_right->m_value);

      std::ostringstream oss;
      oss << std::boolalpha << (l_value != r_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    } else if (on_cond->m_left->m_type == TYPE_VARCHAR
        && on_cond->m_right->m_type == TYPE_VARCHAR)
    {
      std::ostringstream oss;
      oss << std::boolalpha
        << (on_cond->m_left->m_value != on_cond->m_right->m_value);

      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_condition (std::shared_ptr<item> on_cond) {
    switch (on_cond->m_type) {
      case TYPE_ADD:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_add(on_cond);
        break;
      case TYPE_BW_AND:
        break;
      case TYPE_BW_OR:
        break;
      case TYPE_BW_XOR:
        break;
      case TYPE_CMPEQ:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_cmpeq(on_cond);
        break;
      case TYPE_CMPGE:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_cmpge(on_cond);
        break;
      case TYPE_CMPGT:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_cmpgt(on_cond);
        break;
      case TYPE_CMPLE:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_cmple(on_cond);
        break;
      case TYPE_CMPLT:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_cmplt(on_cond);
        break;
      case TYPE_CMPNE:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_cmpne(on_cond);
        break;
      case TYPE_DIV:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_div(on_cond);
        break;
      case TYPE_AND:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_and(on_cond);
        break;
      case TYPE_NOT:
        compact_condition(on_cond->m_right);
        compact_and(on_cond);
        break;
      case TYPE_OR:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_or(on_cond);
        break;
      case TYPE_MOD:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_mod(on_cond);
        break;
      case TYPE_MUL:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_mul(on_cond);
        break;
      case TYPE_ON:
      case TYPE_WHERE:
        compact_condition(on_cond->m_on_cond);
        break;
      case TYPE_SUB:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_sub(on_cond);
        break;
      case TYPE_XOR:
        compact_condition(on_cond->m_left);
        compact_condition(on_cond->m_right);
        compact_xor(on_cond);
        break;
      default:;
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_div (std::shared_ptr<item> on_cond) {
    if ((on_cond->m_left->m_type == TYPE_FLOAT
          || on_cond->m_left->m_type == TYPE_INT)
        && (on_cond->m_right->m_type == TYPE_FLOAT
          || on_cond->m_right->m_type == TYPE_INT))
    {
      float l_value = std::stof(on_cond->m_left->m_value);
      float r_value = std::stof(on_cond->m_right->m_value);

      float value = l_value / r_value;

      on_cond->m_type = TYPE_FLOAT;
      on_cond->m_value = std::to_string(value);
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_mod (std::shared_ptr<item> on_cond) {
  }

  /****************************************************************************
   *
   */
  void vm::compact_mul (std::shared_ptr<item> on_cond) {
    if ((on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_FLOAT)
        || (on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_INT)
        || (on_cond->m_left->m_type == TYPE_INT
          && on_cond->m_right->m_type == TYPE_FLOAT))
    {
      float l_value = std::stof(on_cond->m_left->m_value);
      float r_value = std::stof(on_cond->m_right->m_value);

      float value = l_value * r_value;

      on_cond->m_type = TYPE_FLOAT;
      on_cond->m_value = std::to_string(value);
    } else if (on_cond->m_left->m_type == TYPE_INT
        && on_cond->m_right->m_type == TYPE_INT)
    {
      int32_t l_value = std::stoi(on_cond->m_left->m_value);
      int32_t r_value = std::stoi(on_cond->m_right->m_value);

      int32_t value = l_value * r_value;

      on_cond->m_type = TYPE_INT;
      on_cond->m_value = std::to_string(value);
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_or (std::shared_ptr<item> on_cond) {
    if (on_cond->m_left->m_type == TYPE_BOOLEAN
        && on_cond->m_right->m_type == TYPE_BOOLEAN)
    {
      bool l_value;
      bool r_value;
      std::istringstream(on_cond->m_left->m_value) >> std::boolalpha
        >> l_value;
      std::istringstream(on_cond->m_right->m_value) >> std::boolalpha
        >> r_value;
      std::ostringstream oss;
      oss << std::boolalpha << (l_value || r_value);
      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_sub (std::shared_ptr<item> on_cond) {
    if ((on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_FLOAT)
        || (on_cond->m_left->m_type == TYPE_FLOAT
          && on_cond->m_right->m_type == TYPE_INT)
        || (on_cond->m_left->m_type == TYPE_INT
          && on_cond->m_right->m_type == TYPE_FLOAT))
    {
      float l_value = std::stof(on_cond->m_left->m_value);
      float r_value = std::stof(on_cond->m_right->m_value);

      float value = l_value - r_value;

      on_cond->m_type = TYPE_FLOAT;
      on_cond->m_value = std::to_string(value);
    } else if (on_cond->m_left->m_type == TYPE_INT
        && on_cond->m_right->m_type == TYPE_INT)
    {
      int32_t l_value = std::stoi(on_cond->m_left->m_value);
      int32_t r_value = std::stoi(on_cond->m_right->m_value);

      int32_t value = l_value - r_value;

      on_cond->m_type = TYPE_INT;
      on_cond->m_value = std::to_string(value);
    }
  }

  /****************************************************************************
   *
   */
  void vm::compact_xor (std::shared_ptr<item> on_cond) {
    if (on_cond->m_left->m_type == TYPE_BOOLEAN
        && on_cond->m_right->m_type == TYPE_BOOLEAN)
    {
      bool l_value;
      bool r_value;
      std::istringstream(on_cond->m_left->m_value) >> std::boolalpha
        >> l_value;
      std::istringstream(on_cond->m_right->m_value) >> std::boolalpha
        >> r_value;
      std::ostringstream oss;
      oss << std::boolalpha << ((l_value || r_value) && !(l_value && r_value));
      on_cond->m_type = TYPE_BOOLEAN;
      on_cond->m_value = oss.str();
    }
  }

  /****************************************************************************
   * Interpret an equal-to (=) definition.
   * Construct an comparison (=) item and push onto stack.
   */
  void vm::cmpeq () {
    item* it = new item;
    it->m_type = TYPE_CMPEQ;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   * Interpret an greater-than-equal-to (>=) definition.
   * Construct an comparison (>=) item and push onto stack.
   *
   */
  void vm::cmpge () {
    item* it = new item;
    it->m_type = TYPE_CMPGE;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   * Interpret an greater-than (>) definition.
   * Construct an comparison (>) item and push onto stack.
   *
   */
  void vm::cmpgt () {
    item* it = new item;
    it->m_type = TYPE_CMPGT;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   * Interpret an less-than-equal-to (<=) definition.
   * Construct an comparison (<=) item and push onto stack.
   *
   */
  void vm::cmple () {
    item* it = new item;
    it->m_type = TYPE_CMPLE;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   * Interpret an less-than (<) definition.
   * Construct an comparison (<) item and push onto stack.
   *
   */
  void vm::cmplt () {
    item* it = new item;
    it->m_type = TYPE_CMPLT;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   * Interpret an not-equal-to (!=) definition.
   * Construct an comparison (!=) item and push onto stack.
   *
   */
  void vm::cmpne () {
    item* it = new item;
    it->m_type = TYPE_CMPNE;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   * Iterpret a CREATE DATABASE statement.
   * Create the corresponding database.
   */
  void vm::create_database () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset of database name
      int32_t offset = e->value();
      // verify privilege
      verify_privilege(dbms::ASTERISK, dbms::ASTERISK,
          dbms::CREATE_PRIV);
      // make path to database
      std::ostringstream db_path;
      db_path << m_dbms->path() << '/' << get_varchar(offset + 1);
      // does database exist
      struct stat sb;
      if (stat(db_path.str().c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_CREAT_DATABASE_ERROR)};
      // make directory
      if (mkdir(db_path.str().c_str(),
            (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) < 0)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_SYSTEM_ERROR, errno)};
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 1 row affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   * Interpret a CREATE TABLE statement.
   * Create a schema and table file.
   */
  void vm::create_table () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset of table name
      int32_t offset = e->value();
      // get database name
      std::string db_name = get_varchar(offset + 1);
      if (db_name.empty()) db_name = m_fsm->get_info().db_name();
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset of table name
      offset = e->value();
      // get table name
      std::string tbl_name = get_varchar(offset + 1);
      // verify privilege
      verify_privilege(db_name, tbl_name, dbms::CREATE_PRIV);
      // construct path to table
      std::ostringstream tbl_path;
      tbl_path << m_dbms->path() << '/' << db_name << '/' << tbl_name << ".d";
      // does table already exist
      struct stat sb;
      if (stat(tbl_path.str().c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_CREAT_TABLE_ERROR)};
      // construct schema header
      schema::header header {
          db_name, tbl_name, 0, tree::BUFFER_SZ,
          static_cast<int32_t>(m_wrk_sch->m_index.size()),
          static_cast<int32_t>(m_wrk_sch->m_foreign.size()),
          static_cast<int32_t>(m_wrk_sch->m_column.size()), 0L, -1L, -1L};
      // make schema
      schema::schema* sch = new schema::schema {header,
        m_wrk_sch->m_column, m_wrk_sch->m_index, m_wrk_sch->m_foreign};
      // save schema
      std::string sch_name = db_name + '.' + tbl_name;
      m_dbms->get_schema_mgr()->add_schema(sch_name, sch);
      m_dbms->get_schema_mgr()->save_schema(sch_name);
      // make file path
      std::ostringstream path;
      path << m_dbms->path() << '/' << db_name << '/' << tbl_name << ".d";
      // create file
      dio dio {true, path.str()};
      // add referenced-by objects to schema for foreign tables
      make_reference (db_name, tbl_name, m_wrk_sch->m_foreign);
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::create_user () {
    try {
      // lock mutex to prevent race condition
      // TODO: m_dbms->unique_lock_userd_mutex();
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get username
      std::string username = get_varchar(offset + 1);
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset
      offset = e->value();
      // get hostname
      std::string hostname = get_varchar(offset + 1);
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset
      offset = e->value();
      // get password
      std::string password = get_varchar(offset + 1);
      // verify privilege
      verify_privilege(dbms::ASTERISK, dbms::ASTERISK,
          dbms::CREATE_USER_PRIV);
      // construct block-cache name (schema-name)
      std::ostringstream bc_name;
      bc_name << dbms::ETC_DIR << '.' << dbms::CRED_TBL;
      // get block-cache
      auto blk_cache = m_dbms->get_block_cache(bc_name.str());
      // if the block-cache was not found throw exceptoin
      if (blk_cache == nullptr)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_TABLE_REF)};
      // construct tree object
      auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
      // unique-lock tree
      the_tree->set_unique_lock();
      // construct row
      auto rw = the_tree->make_row();
      // populate row
      rw.append(0, hostname);
      rw.append(1, username);
      // construct key
      std::string key = rw.make_key(schema::PK_NDX);
      // does user already exist
      auto iter = the_tree->find(schema::PK_NDX, key);
      if (iter != the_tree->end(schema::PK_NDX))
        throw std::runtime_error{eno::get_msg(eno::ERRNO_CREAT_USER_ERROR)};
      // 2020-01-24: fix to prevent plain text password from being prepended
      // to hashed password
      std::string hashed_password;
      // hash password
      if (not m_crypto->hash_text(hashed_password, password))
        throw std::runtime_error{eno::get_msg(eno::ERRNO_CRYPTO_ERROR)};
      // construct row
      auto a_row = the_tree->make_row();
      // populate row
      a_row.append(0, hostname);
      a_row.append(1, username);
      a_row.append(2, hashed_password);
      for (int32_t i = 3;
          i < a_row.get_schema()->get_header().column_count(); ++i)
        a_row.append(i, "N");
      // insert row into tree
      the_tree->insert(a_row);
      // flush tree to disk
      m_dbms->flush(the_tree);
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::cross_join () {
    if (m_stack.top()->m_type != red::TYPE_ON)
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_JOIN)};
    // pop on/using condition (item) off stack
    auto on_cond = m_stack.top(); m_stack.pop();
    // compact on condition
    compact_condition(on_cond);
    // if no join exists pop tree (item) off stack
    if (m_head == nullptr) {
      m_head = m_tail = m_stack.top();
      m_stack.pop();
    }
    // set action
    m_tail->m_action = TYPE_CRJOIN;
    // pop tree (item) off stack
    m_tail->m_next = m_stack.top(); m_stack.pop();
    m_tail->m_next->m_prev = m_tail;
    m_tail = m_tail->m_next;
    m_tail->m_on_cond = on_cond;
  }

  /****************************************************************************
   *
   */
  void vm::delete_row () {
    try {
      // get corresponding tree
      auto the_tree = m_tree_list.back()->tree();
      if (the_tree == nullptr)
        throw std::runtime_error{eno::get_msg(eno::ERRNO_MALFORMED_TABLE_REF)};
      // verify privilege
      verify_privilege(the_tree->db_name(), the_tree->tbl_name(),
          dbms::DELETE_PRIV);
      // find matching row(s)
      std::vector<std::string> key_list;
      for (auto iter = the_tree->begin(); iter != the_tree->end(); ++iter) {
        auto a_row = *iter;
        set_column_value(m_where, a_row);
        std::string res;
        exec_condition(res, m_where);
        if (res == "true") {
          std::string key = a_row.make_key(schema::PK_NDX);
          key_list.push_back(key);
        }
      }
      // delete matching row(s)
      size_t row_count {0L};
      for (auto key : key_list) {
        auto iter = the_tree->find(schema::PK_NDX, key);
        if (iter != the_tree->end(schema::PK_NDX)) {
          auto a_row = *iter;
          the_tree->remove(a_row);
          ++row_count;
        }
      }
      m_dbms->flush(the_tree);
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, " << row_count << " rows affected (" << std::fixed
        << std::setw(4) << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void calc_column_meta (
      std::vector<std::pair<std::string, size_t> >& meta_list,
      std::vector<std::pair<std::string, std::string> >& col_value_list,
      std::vector<schema::column>& column_list)
  {
    for (auto& col : column_list) {
      if (meta_list[FLD_FIELD].second < col.name().size())
        meta_list[FLD_FIELD].second = col.name().size();
      std::ostringstream type;
      switch (col.type()) {
        case red::schema::TYPE_BOOLEAN:
          break;
        case red::schema::TYPE_CHAR:
          type << "char(" << col.size() << ')';
          if (meta_list[FLD_TYPE].second < type.str().size())
            meta_list[FLD_TYPE].second = type.str().size();
          break;
        case red::schema::TYPE_BINARY:
          type << "binary(" << col.size() << ')';
          if (meta_list[FLD_TYPE].second < type.str().size())
            meta_list[FLD_TYPE].second = type.str().size();
          break;
        case red::schema::TYPE_DATE:
          type << "date";
          if (meta_list[FLD_TYPE].second < type.str().size())
            meta_list[FLD_TYPE].second = type.str().size();
          break;
        case red::schema::TYPE_DATETIME:
          type << "datetime";
          if (meta_list[FLD_TYPE].second < type.str().size())
            meta_list[FLD_TYPE].second = type.str().size();
          break;
        case red::schema::TYPE_ENUM:
          type << "enum";
          if (meta_list[FLD_TYPE].second < type.str().size())
            meta_list[FLD_TYPE].second = type.str().size();
          break;
        case red::schema::TYPE_FLOAT:
          type << "float(" << col.size() << ',' << col.prec() << ')';
          if (meta_list[FLD_TYPE].second < type.str().size())
            meta_list[FLD_TYPE].second = type.str().size();
          break;
        case red::schema::TYPE_INT:
          type << "int(" << col.size() << ')';
          if (meta_list[FLD_TYPE].second < type.str().size())
            meta_list[FLD_TYPE].second = type.str().size();
          break;
        case red::schema::TYPE_TIMESTAMP:
          type << "timestamp";
          if (meta_list[FLD_TYPE].second < type.str().size())
            meta_list[FLD_TYPE].second = type.str().size();
          break;
        case red::schema::TYPE_VARCHAR:
          type << "varchar(" << col.size() << ')';
          if (meta_list[FLD_TYPE].second < type.str().size())
            meta_list[FLD_TYPE].second = type.str().size();
          break;
        default:;
      }
      std::string auto_inc;
      if ((col.flag() & schema::FLAG_AUTO_INC) == schema::FLAG_AUTO_INC) {
        meta_list[FLD_EXTRA].second = AUTO_INCREMENT.size();
        auto_inc = AUTO_INCREMENT;
      }
      col_value_list.emplace_back(type.str(), auto_inc);
      type.str("");
    }
  }

  /****************************************************************************
   *
   */
  void vm::describe_column () {
    // get the tree
    auto the_tree = m_tree_list.back()->tree();
    // construct meta list
    std::vector<std::pair<std::string, size_t> > meta_list = COL_META_LIST;
    // construct column list
    auto& column_list = the_tree->get_schema()->get_column_list();
    // update meta list
    std::vector<std::pair<std::string, std::string> > value_list;
    calc_column_meta(meta_list, value_list, column_list);
    // output column info
    std::ostringstream title;
    title << VB;
    for (size_t i = 0L; i < meta_list.size(); ++i) {
      title << SPC;
      title << std::left << std::setw(meta_list[i].second)
        << std::setfill(' ') << meta_list[i].first << SPC <<  VB;
    }
    title << NL;

    std::string barbwire {PS};
    for (size_t i = 0L; i < meta_list.size(); ++i) {
      barbwire.append(meta_list[i].second + 2, MS);
      barbwire += PS;
    }
    barbwire += NL;

    m_fsm->reset(red::comm::OP_RSLT_SYN, red::comm::TYPE_TXT);
    m_fsm->put(barbwire);
    m_fsm->put(title.str());
    m_fsm->put(barbwire);

    for (size_t i = 0L; i < column_list.size(); ++i) {
      std::string line;
      line += VB;
      line += SPC;
      std::ostringstream oss2;
      oss2 << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_FIELD].second))
        << std::setfill(' ') << column_list[i].name();
      line += oss2.str();
      line += SPC;
      line += VB;
      line += SPC;
      oss2.str("");
      oss2 << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_TYPE].second))
        << std::setfill(' ') << value_list[i].first;
      line += oss2.str();
      line += SPC;
      line += VB;
      line += SPC;
      oss2.str("");
      oss2 << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_NULL].second))
        << std::setfill(' ') << (((column_list[i].flag() & schema::FLAG_NOT_NULL)
            == schema::FLAG_NOT_NULL) ? "No" : "Yes");
      line += oss2.str();
      line += SPC;
      line += VB;
      line += SPC;
      oss2.str("");
      oss2 << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_PRIKEY].second))
        << std::setfill(' ') << (((column_list[i].flag() & schema::FLAG_PRIMARY)
            != schema::FLAG_PRIMARY) ? "No" : "Yes");
      line += oss2.str();
      line += SPC;
      line += VB;
      line += SPC;
      oss2.str("");
      oss2 << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_UNIKEY].second))
        << std::setfill(' ') << (((column_list[i].flag() & schema::FLAG_UNIQUE)
            != schema::FLAG_UNIQUE) ? "No" : "Yes");
      line += oss2.str();
      line += SPC;
      line += VB;
      line += SPC;
      oss2.str("");
      oss2 << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_MULKEY].second))
        << std::setfill(' ') << (((column_list[i].flag() & schema::FLAG_MULTIPLE)
            != schema::FLAG_MULTIPLE) ? "No" : "Yes");
      line += oss2.str();
      line += SPC;
      line += VB;
      line += SPC;
      oss2.str("");
      oss2 << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_EXTRA].second))
        << std::setfill(' ') << value_list[i].second;
      line += oss2.str();
      line += SPC;
      line += VB;
      line += NL;
      m_fsm->put(line);
    }

    m_fsm->put(barbwire);

    std::ostringstream oss;
    oss << the_tree->get_schema()->get_column_list().size()
      << " rows in set\n\n";
    m_fsm->put(oss.str());
  }

  /****************************************************************************
   *
   */
  void calc_key_meta (std::string const& tree_name,
      std::vector<std::pair<std::string, size_t> >& meta_list,
      std::vector<schema::index>& ndx_list,
      std::vector<schema::foreign>& fgn_list)
  {
    for (auto ndx : ndx_list) {
      if (meta_list[FLD_KEY].second < ndx.name().size())
        meta_list[FLD_KEY].second = ndx.name().size();
      size_t size {0L};
      for (auto col_name : ndx.col_name_list())
        size += col_name.size() + 2;
      size -= 2;
      if (meta_list[FLD_COL].second < size)
        meta_list[FLD_COL].second = size;
      if (meta_list[FLD_TBL].second < tree_name.size())
        meta_list[FLD_TBL].second = tree_name.size();
    }
    // update meta list
    for (auto fgn : fgn_list) {
      if (meta_list[FLD_KEY].second < fgn.name().size())
        meta_list[FLD_KEY].second = fgn.name().size();
      size_t size {0L};
      for (auto col_name : fgn.col_name_list())
        size += col_name.size() + 2;
      size -= 2;
      if (meta_list[FLD_COL].second < size)
        meta_list[FLD_COL].second = size;
      std::ostringstream tbl_name;
      tbl_name << fgn.db_name() << '.' << fgn.tbl_name();
      if (meta_list[FLD_TBL].second < tbl_name.str().size())
        meta_list[FLD_TBL].second = tbl_name.str().size();
    }
  }

  /****************************************************************************
   *
   */
  void vm::describe_key () {
    // get the tree
    auto the_tree = m_tree_list.back()->tree();
    // construct index list
    auto& ndx_list = the_tree->get_schema()->get_index_list();
    // construct foreign-key list
    auto& fgn_list = the_tree->get_schema()->get_foreign_list();
    // construct meta_list
    auto meta_list = NDX_META_LIST;
    // update meta list
    calc_key_meta(the_tree->name(), meta_list, ndx_list, fgn_list);
    // output key info
    std::ostringstream title;
    title << VB;
    for (size_t i = 0L; i < meta_list.size(); ++i) {
      title << SPC;
      title << std::left << std::setw(meta_list[i].second)
        << std::setfill(' ') << meta_list[i].first << SPC <<  VB;
    }
    title << NL;

    std::string barbwire {PS};
    for (size_t i = 0L; i < meta_list.size(); ++i) {
      barbwire.append(meta_list[i].second + 2, MS);
      barbwire += PS;
    }
    barbwire += NL;

//    m_fsm->reset(red::comm::OP_RSLT_SYN, red::comm::TYPE_TXT);
    m_fsm->put(barbwire);
    m_fsm->put(title.str());
    m_fsm->put(barbwire);
    // output index info
    for (auto ndx : ndx_list) {
      std::string line;
      line += VB;
      line += SPC;
      std::ostringstream oss;
      oss << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_KEY].second))
        << std::setfill(' ') << ndx.name();
      line += oss.str();
      line += SPC;
      line += VB;
      line += SPC;
      oss.str("");
      std::string type;
      if ((ndx.flag() & schema::FLAG_PRIMARY) == schema::FLAG_PRIMARY)
        type = "PRI";
      else if ((ndx.flag() & schema::FLAG_UNIQUE) == schema::FLAG_UNIQUE)
        type = "UNI";
      else if ((ndx.flag() & schema::FLAG_MULTIPLE) == schema::FLAG_MULTIPLE)
        type = "MUL";
      oss << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_TYPE].second))
        << std::setfill(' ') << type;
      line += oss.str();
      line += SPC;
      line += VB;
      line += SPC;
      std::string name_list;
      for (auto col_name : ndx.col_name_list()) {
        name_list += col_name;
        name_list += ", ";
      }
      name_list.pop_back();
      name_list.pop_back();
      oss.str("");
      oss << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_COL].second))
        << std::setfill(' ') << name_list;
      line += oss.str();
      line += SPC;
      line += VB;
      line += SPC;
      oss.str("");
      oss << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_TBL].second))
        << std::setfill(' ') << the_tree->name();
      line += oss.str();
      line += SPC;
      line += VB;
      line += NL;
      m_fsm->put(line);
    }
    // output foreign-key info
    for (auto fgn : fgn_list) {
      std::string line;
      line += VB;
      line += SPC;
      std::ostringstream oss;
      oss << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_KEY].second))
        << std::setfill(' ') << fgn.name();
      line += oss.str();
      line += SPC;
      line += VB;
      line += SPC;
      oss.str("");
      oss << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_TYPE].second))
        << std::setfill(' ') << "FGN";
      line += oss.str();
      line += SPC;
      line += VB;
      line += SPC;
      std::string name_list;
      for (auto col_name : fgn.col_name_list()) {
        name_list += col_name;
        name_list += ", ";
      }
      name_list.pop_back();
      name_list.pop_back();
      oss.str("");
      oss << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_COL].second))
        << std::setfill(' ') << name_list;
      line += oss.str();
      line += SPC;
      line += VB;
      line += SPC;
      name_list.clear();
      name_list += fgn.db_name();
      name_list += '.';
      name_list += fgn.tbl_name();
      oss.str("");
      oss << std::left
        << std::setw(static_cast<int32_t>(meta_list[FLD_TBL].second))
        << std::setfill(' ') << name_list;
      line += oss.str();
      line += SPC;
      line += VB;
      line += NL;
      m_fsm->put(line);
    }

    m_fsm->put(barbwire);
  }

  /****************************************************************************
   *
   */
  void vm::describe_table () {
    try {
      // get the tree
      auto the_tree = m_tree_list.back()->tree();
      // describe column(s)
      describe_column();
      // describe key(s)
      describe_key();
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      size_t row_count {0L};
      row_count = the_tree->get_schema()->get_index_list().size()
        + the_tree->get_schema()->get_foreign_list().size();
      std::ostringstream oss;
      oss << row_count << " rows in set ("
        << std::fixed << std::setw(4) << std::setprecision(2) << fs.count()
        << " sec)\n";
      if (oss.str().size() > m_fsm->remaining()) m_fsm->flush();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   * Interpret a division (/) definition.
   * Construct an mathematical division (/) item and push onto stack.
   */
  void vm::div () {
    item* it = new item;
    it->m_type = TYPE_DIV;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   *
   */
  void vm::drop_column () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get column name
      std::string col_name = get_varchar(offset + 1);
      // get tree
      auto the_tree = m_tree_list.back()->tree();
      // verify privilege
      verify_privilege(the_tree->get_schema()->get_header().db_name(),
          the_tree->get_schema()->get_header().tbl_name(), dbms::ALTER_PRIV);
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << ">>> ALTER TABLE <name> DROP COLUMN is not currently supported.";
      oss << " [Target Version: 2.0]\n";
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   * Interpret a DROP DATABASE db_name statement.
   * Drop the corresponding database (Must be empty).
   */
  void vm::drop_database () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get database name
      std::string db_name = get_varchar(offset + 1);
      // verify privilege
      verify_privilege(db_name, dbms::ASTERISK, dbms::DROP_PRIV);
      // consturct path to database
      std::ostringstream db_path;
      db_path << m_dbms->path() << '/' << db_name;
      // does database exist
      DIR* dir;
      if ((dir = opendir(db_path.str().c_str())) == nullptr)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_DATABASE_REF)};
      // get maximum length of a filename in the directory path
      // calculate size of dirent structure
      auto size = offsetof(struct dirent, d_name) + FILENAME_MAX + 1;
      // allocate directory entry structure
      struct dirent* de = reinterpret_cast<struct dirent*>(calloc(1, size));
      // populate table-name list
      std::vector<std::string> tbl_name_list;
      struct dirent* res {nullptr};
      while (readdir_r(dir, de, &res) == 0 && res != nullptr)
      {
        std::string tbl_name {de->d_name};
#ifdef _DIRENT_HAVE_D_TYPE
        if (de->d_type == DT_REG) {
          // remove ".d" (or whatever) suffix
          tbl_name.resize(tbl_name.size() - 2);
          // push tbl_name onto list
          tbl_name_list.push_back(tbl_name);
        }
#else
        std::ostringstream file_path;
        file_path << db_path.str() << '/' << de->d_name;
        struct stat sb;
        if (stat(file_path.str().c_str(), &sb) < 0)
          throw std::runtime_error{eno::get_msg(
              eno::ERRNO_SYSTEM_ERROR, errno)};
        if (S_ISREG(sb.st_mode)) {
          // remove ".d" (or whatever) suffix
          tbl_name.resize(tbl_name.size() - 2);
          // push tbl_name onto list
          tbl_name_list.push_back(tbl_name);
        }
#endif
      }
      // close directory
      closedir(dir);
      // drop block-cache(s) from file-cache
      for (auto tbl_name :tbl_name_list) {
        std::ostringstream bc_name;
        bc_name << db_name << '.' << tbl_name;
        m_dbms->drop_block_cache(bc_name.str());
      }
      // drop table(s) from database
      for (auto tbl_name : tbl_name_list) {
        // construct path to database
        std::ostringstream tbl_path;
        tbl_path << db_path.str() << '/' << tbl_name << ".d";
        // drop table
        errno = 0;
        if (unlink(tbl_path.str().c_str()) < 0)
          throw std::runtime_error{eno::get_msg(
              eno::ERRNO_SYSTEM_ERROR, errno)};
        // construct schema path
        std::ostringstream sch_path;
        sch_path << m_dbms->path() << '/' << dbms::ETC_DIR << '/'
          << db_name << '.' << tbl_name << ".json";
        // drop schema
        errno = 0;
        if (unlink(sch_path.str().c_str()) < 0)
          throw std::runtime_error{eno::get_msg(
              eno::ERRNO_SYSTEM_ERROR, errno)};
      }
      // drop (directory) database
      errno = 0;
      if (rmdir(db_path.str().c_str()) < 0)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_SYSTEM_ERROR, errno)};
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, " << tbl_name_list.size() << " rows affected ("
        << std::fixed << std::setw(4) << std::setprecision(2) << fs.count()
        << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::drop_foreign_key () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get index name
      std::string fgn_name = get_varchar(offset + 1);
      // get tree (unique-lock should already be set)
      auto the_tree = m_tree_list.back()->tree();
      // verify privilege
      verify_privilege(the_tree->db_name(), the_tree->tbl_name(),
          dbms::ALTER_PRIV);
      // get iterator to foreign key
      auto iter = std::find_if(the_tree->get_schema()->get_foreign_list()
          .begin(), the_tree->get_schema()->get_foreign_list().end(),
          [&](auto fgn){return(fgn.name() == fgn_name);});
      if (iter == the_tree->get_schema()->get_foreign_list().end())
        throw std::runtime_error{
          eno::get_msg(eno::ERRNO_MALFORMED_FOREIGN_KEY)};
      // verify privilege
      verify_privilege((*iter).db_name(), (*iter).tbl_name(),
          dbms::ALTER_PRIV);
      // construct block-cache name (schema-name)
      std::ostringstream bc_name;
      bc_name << (*iter).db_name() << '.' << (*iter).tbl_name();
      // get block-cache
      auto blk_cache = m_dbms->get_block_cache(bc_name.str());
      // if the block-cache was not found throw exceptoin
      if (blk_cache == nullptr)
        throw std::runtime_error{eno::get_msg(eno::ERRNO_MALFORMED_TABLE_REF)};
      // construct foreign tree
      auto fgn_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
      // set unique-lock on foreign tree
      fgn_tree->set_unique_lock();
      // get foreign key from iterator
      auto fgn = *iter;
      // remove foreign key from schema
      the_tree->get_schema()->get_foreign_list().erase(iter);
      // remove reference(s) from foreign schema -- brutal
      for (auto& col_name : fgn.col_name_list()) {
        auto col_iter = std::find_if(fgn_tree->get_schema()->get_column_list()
            .begin(), fgn_tree->get_schema()->get_column_list().end(),
            [&](auto& col){return(col.name() == col_name);});
        if (col_iter == fgn_tree->get_schema()->get_column_list().end())
          throw std::runtime_error{
            eno::get_msg(eno::ERRNO_MALFORMED_REFERENCE)};
        auto ref_iter = std::find_if((*col_iter).reference_list().begin(),
            (*col_iter).reference_list().end(),
            [&](auto& ref){return(ref.fgn_name() == fgn.name());});
        if (ref_iter == (*col_iter).reference_list().end())
          throw std::runtime_error{
            eno::get_msg(eno::ERRNO_MALFORMED_REFERENCE)};
        (*col_iter).reference_list().erase(ref_iter);
      }
      // save local schema
      m_dbms->get_schema_mgr()->save_schema(the_tree->name());
      // save foreign schema
      m_dbms->get_schema_mgr()->save_schema(fgn_tree->name());
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::drop_index () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get index name
      std::string ndx_name = get_varchar(offset + 1);
      // block dropping primary-key index
      if (ndx_name == red::schema::PK_NDX)
        throw std::runtime_error{eno::get_msg(eno::ERRNO_DROP_PRIMARY_KEY)};
      // get tree
      auto the_tree = m_tree_list.back()->tree();
      // verify privilege
      verify_privilege(the_tree->get_schema()->get_header().db_name(),
          the_tree->get_schema()->get_header().tbl_name(), dbms::ALTER_PRIV);
      // construct remove-index object and remove index
      tree::remove_index{ndx_name, the_tree}.remove();
      // flush the tree
      m_dbms->flush(the_tree);
      // save updated schema
      m_dbms->get_schema_mgr()->save_schema(the_tree->name());
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   * Interpret a DROP TABLE tbl_name statement.
   * Drop the corresponding table.
   */
  void vm::drop_table () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get database name
      std::string db_name = get_varchar(offset + 1);
      if (db_name.empty()) db_name = m_fsm->get_info().db_name();
      if (db_name.empty())
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_NO_DATABASE_ERROR)};
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset
      offset = e->value();
      // get table name
      std::string tbl_name = get_varchar(offset + 1);
      // verify privilege
      verify_privilege(db_name, tbl_name, dbms::DROP_PRIV);
      // make path to database
      std::ostringstream tbl_path;
      tbl_path << m_dbms->path() << '/' << db_name << '/' << tbl_name << ".d";
      // delete table
      if (unlink(tbl_path.str().c_str()) < 0)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_SYSTEM_ERROR, errno)};
      // make schema path
      std::ostringstream sch_path;
      sch_path << m_dbms->path() << '/' << dbms::ETC_DIR << '/'
        << db_name << '.' << tbl_name << ".json";
      // delete schema
      if (unlink(sch_path.str().c_str()) < 0)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_SYSTEM_ERROR, errno)};
      // construct file-cache name
      std::stringstream tc_name;
      tc_name << db_name << '.' << tbl_name;
      // drop file-cache
      m_dbms->drop_block_cache(tc_name.str());
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::drop_user () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get username
      std::string username = get_varchar(offset + 1);
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset
      offset = e->value();
      // get hostname
      std::string hostname = get_varchar(offset + 1);
      // verify privilege
      std::vector<std::string> priv_list {dbms::CREATE_USER_PRIV,
        dbms::DELETE_PRIV};
      verify_privilege(dbms::ASTERISK, dbms::ASTERISK, priv_list);
      // drop user from etc.userdata
      drop_user_global(hostname, username);
      // drop user privileges from etc.priv
      drop_user_local(hostname, username);
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::drop_user_global (std::string const& hostname,
      std::string const& username)
  {
    try {
      // verify username@hostname
      // construct schema-name
      std::ostringstream sch_name;
      sch_name << dbms::ETC_DIR << '.' << dbms::CRED_TBL;
      // get block-cache object
      auto blk_cache = m_dbms->get_block_cache(sch_name.str());
      // construct tree object
      auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
      // unique-lock tree
      the_tree->set_unique_lock();
      // construct row object
      auto rw = the_tree->make_row();
      // populate row
      rw.append(0, hostname);
      rw.append(1, username);
      // make primary-key
      std::string key = rw.make_key(schema::PK_NDX);
      // find corresponding row, if exists
      auto iter = the_tree->find(schema::PK_NDX, key);
      // row does not exist
      if (iter == the_tree->end(schema::PK_NDX))
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_NAME)};
      // delete user from etc.userdata
      rw = *iter;
      the_tree->remove(rw);
      // flush tree
      m_dbms->flush(the_tree);
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::drop_user_local (std::string const& hostname,
      std::string const& username)
  {
    try {
      // construct schema-name
      std::ostringstream sch_name;
      sch_name << dbms::ETC_DIR << '.' << dbms::PRIV_TBL;
      // get block-cache object
      auto blk_cache = m_dbms->get_block_cache(sch_name.str());
      // construct tree object
      auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
      // unique-lock tree
      the_tree->set_unique_lock();
      // find corresponding privilege(s), if exist
      std::vector<std::string> key_list;
      for (auto iter = the_tree->begin(); iter != the_tree->end(); ++iter) {
        auto rw = *iter;
        if (rw.value_at(0) == hostname && rw.value_at(1) == username) {
          auto key = rw.make_key(schema::PK_NDX);
          key_list.push_back(key);
        }
      }
      // delete privileges from etc.priv
      for (auto key : key_list) {
        auto iter = the_tree->find(schema::PK_NDX, key);
        if (iter != the_tree->end(schema::PK_NDX)) {
          auto rw = *iter;
          the_tree->remove(rw);
        }
      }
      // flush tree
      m_dbms->flush(the_tree);
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::exec_add (std::string& res, std::shared_ptr<item> oper)
  {
    try {
      std::string l_res;
      std::string r_res;
      exec_condition(l_res, oper->m_left);
      if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
      exec_condition(r_res, oper->m_right);
      if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
      float l_value;
      float r_value;
      if (is_number(l_value, l_res) && is_number(r_value, r_res)) {
        std::ostringstream oss;
        oss << (l_value + r_value);
        res = oss.str();
      } else throw std::runtime_error{
        eno::get_msg(eno::ERRNO_MALFORMED_ADDITION)};
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::exec_and (std::string& res, std::shared_ptr<item> oper)
  {
    std::string l_res;
    std::string r_res;
    exec_condition(l_res, oper->m_left);
    if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
    bool l_value;
    std::istringstream(l_res) >> std::boolalpha >> l_value;
    if (not l_value) { res = l_res; return; }
    exec_condition(r_res, oper->m_right);
    if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
    bool r_value;
    std::istringstream(r_res) >> std::boolalpha >> r_value;
    if (not r_value) { res = r_res; return; }
    res = red::VM_TRUE;
  }

  /****************************************************************************
   *
   */
  void vm::exec_assignment (std::string& res, std::shared_ptr<item> it)
  {
    switch (it->m_type) {
      case TYPE_ADD:
        exec_add(res, it);
        break;
      case TYPE_ASSIGN:
        exec_assignment(res, it->m_right);
        break;
      case TYPE_BW_AND:
        break;
      case TYPE_BW_OR:
        break;
      case TYPE_BW_XOR:
        break;
      case TYPE_CMPEQ:
        exec_cmpeq(res, it);
        break;
      case TYPE_CMPGE:
        exec_cmpge(res, it);
        break;
      case TYPE_CMPGT:
        exec_cmpgt(res, it);
        break;
      case TYPE_CMPLE:
        exec_cmple(res, it);
        break;
      case TYPE_CMPLT:
        exec_cmplt(res, it);
        break;
      case TYPE_CMPNE:
        exec_cmpne(res, it);
        break;
      case TYPE_DIV:
        exec_div(res, it);
        break;
      case TYPE_FLOAT:
      case TYPE_INT:
        res = it->m_value;
        break;
      case TYPE_AND:
        exec_and(res, it);
        break;
      case TYPE_NOT:
        exec_and(res, it);
        break;
      case TYPE_OR:
        exec_or(res, it);
        break;
      case TYPE_MOD:
        exec_mod(res, it);
        break;
      case TYPE_MUL:
        exec_mul(res, it);
        break;
      case TYPE_SUB:
        exec_sub(res, it);
        break;
      case TYPE_VARCHAR:
        res = it->m_value;
        break;
      case TYPE_XOR:
        exec_xor(res, it);
        break;
      default:;
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_cmpeq (std::string& res, std::shared_ptr<item> oper)
  {
    std::string l_res;
    std::string r_res;
    exec_condition(l_res, oper->m_left);
    if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
    exec_condition(r_res, oper->m_right);
    if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
    float l_value;
    float r_value;
    if (is_number(l_value, l_res) && is_number(r_value, r_res)) {
      std::ostringstream oss;
      oss << std::boolalpha << (l_value == r_value);
      res = oss.str();
    } else {
      std::ostringstream oss;
      oss << std::boolalpha << (l_res == r_res);
      res = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_cmpge (std::string& res, std::shared_ptr<item> oper)
  {
    std::string l_res;
    std::string r_res;
    exec_condition(l_res, oper->m_left);
    if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
    exec_condition(r_res, oper->m_right);
    if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
    float l_value;
    float r_value;
    if (is_number(l_value, l_res) && is_number(r_value, r_res)) {
      std::ostringstream oss;
      oss << std::boolalpha << (l_value >= r_value);
      res = oss.str();
    } else {
      std::ostringstream oss;
      oss << std::boolalpha << (l_res >= r_res);
      res = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_cmpgt (std::string& res, std::shared_ptr<item> oper)
  {
    std::string l_res;
    std::string r_res;
    exec_condition(l_res, oper->m_left);
    if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
    exec_condition(r_res, oper->m_right);
    if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
    float l_value;
    float r_value;
    if (is_number(l_value, l_res) && is_number(r_value, r_res)) {
      float l_value = std::stof(l_res);
      float r_value = std::stof(r_res);
      std::ostringstream oss;
      oss << std::boolalpha << (l_value > r_value);
      res = oss.str();
    } else {
      std::ostringstream oss;
      oss << std::boolalpha << (l_res > r_res);
      res = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_cmple (std::string& res, std::shared_ptr<item> oper)
  {
    std::string l_res;
    std::string r_res;
    exec_condition(l_res, oper->m_left);
    if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
    exec_condition(r_res, oper->m_right);
    if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
    float l_value;
    float r_value;
    if (is_number(l_value, l_res) && is_number(r_value, r_res)) {
      float l_value = std::stof(l_res);
      float r_value = std::stof(r_res);
      std::ostringstream oss;
      oss << std::boolalpha << (l_value <= r_value);
      res = oss.str();
    } else {
      std::ostringstream oss;
      oss << std::boolalpha << (l_res <= r_res);
      res = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_cmplt (std::string& res, std::shared_ptr<item> oper)
  {
    std::string l_res;
    std::string r_res;
    exec_condition(l_res, oper->m_left);
    if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
    exec_condition(r_res, oper->m_right);
    if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
    float l_value;
    float r_value;
    if (is_number(l_value, l_res) && is_number(r_value, r_res)) {
      float l_value = std::stof(l_res);
      float r_value = std::stof(r_res);
      std::ostringstream oss;
      oss << std::boolalpha << (l_value < r_value);
      res = oss.str();
    } else {
      std::ostringstream oss;
      oss << std::boolalpha << (l_res < r_res);
      res = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_cmpne (std::string& res, std::shared_ptr<item> oper)
  {
    std::string l_res;
    std::string r_res;
    exec_condition(l_res, oper->m_left);
    if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
    exec_condition(r_res, oper->m_right);
    if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
    float l_value;
    float r_value;
    if (is_number(l_value, l_res) && is_number(r_value, r_res)) {
      float l_value = std::stof(l_res);
      float r_value = std::stof(r_res);
      std::ostringstream oss;
      oss << std::boolalpha << (l_value != r_value);
      res = oss.str();
    } else {
      std::ostringstream oss;
      oss << std::boolalpha << (l_res != r_res);
      res = oss.str();
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_condition (std::string& res, std::shared_ptr<item> oper)
  {
    if (oper == nullptr) return;

    switch (oper->m_type) {
      case TYPE_NULL: res = "true"; break;
      case TYPE_ADD: exec_add(res, oper); break;
      case TYPE_BOOLEAN:
        if (oper->m_value != NULL_VALUE) {
          std::istringstream(oper->m_value) >> std::boolalpha >> res;
        } else res = NULL_VALUE;
        break;
      case TYPE_BW_AND: break;
      case TYPE_BW_OR: break;
      case TYPE_BW_XOR: break;
      case TYPE_COLUMN: res = oper->m_value; break;
      case TYPE_CMPEQ: exec_cmpeq(res, oper); break;
      case TYPE_CMPGE: exec_cmpge(res, oper); break;
      case TYPE_CMPGT: exec_cmpgt(res, oper); break;
      case TYPE_CMPLE: exec_cmple(res, oper); break;
      case TYPE_CMPLT: exec_cmplt(res, oper); break;
      case TYPE_CMPNE: exec_cmpne(res, oper); break;
      case TYPE_DIV: exec_div(res, oper); break;
      case TYPE_FLOAT:
      case TYPE_INT: res = oper->m_value; break;
      case TYPE_AND: exec_and(res, oper); break;
      case TYPE_NOT: break;
      case TYPE_OR: exec_or(res, oper); break;
      case TYPE_MOD: exec_mod(res, oper); break;
      case TYPE_MUL: exec_mul(res, oper); break;
      case TYPE_ON:
      case TYPE_WHERE: exec_condition(res, oper->m_on_cond); break;
      case TYPE_SUB: exec_sub(res, oper); break;
      case TYPE_VARCHAR: res = oper->m_value; break;
      case TYPE_XOR: exec_xor(res, oper); break;
      default:
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_COMPARISON)};
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_div (std::string& res, std::shared_ptr<item> oper)
  {
    try {
      std::string l_res;
      std::string r_res;
      exec_condition(l_res, oper->m_left);
      if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
      exec_condition(r_res, oper->m_right);
      if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
      float l_value;
      float r_value;
      if (is_number(l_value, l_res) && is_number(r_value, r_res)) {
        if (r_value == 0.0)
          throw std::runtime_error{eno::get_msg(eno::ERRNO_DIVIDE_BY_ZERO)};
        std::ostringstream oss;
        oss << (l_value / r_value);
        res = oss.str();
      } else throw std::runtime_error{
        eno::get_msg(eno::ERRNO_MALFORMED_DIVISION)};
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::exec_inner_join (std::shared_ptr<item> curr) {
    auto prev = curr->m_prev;

    if (prev->m_prev != nullptr) {
      if (curr->m_ndx_name.empty()) exec_inner_join_x_0(curr);
      else exec_inner_join_x_1(curr);
    } else {
      if (curr->m_ndx_name.empty()) exec_inner_join_0_0(curr);
      else exec_inner_join_0_1(curr);
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_inner_join_x_0 (std::shared_ptr<item> curr)
  {
    auto prev = (curr->m_opt_prev == nullptr ? curr->m_prev : curr->m_opt_prev);
    int32_t count {0};
    for (curr->m_diter = curr->tree()->begin();
        curr->m_diter != curr->tree()->end();
        ++curr->m_diter)
    {
      yield();

      curr->m_row = *curr->m_diter;

      set_column_value(curr->m_on_cond, prev->m_row, curr->m_row);

      std::string res;
      exec_condition(res, curr->m_on_cond);

      if (res == "true") {
        ++count;
        for (auto it : m_expr_list) set_column_value(it, curr->m_row);
        set_column_value(m_where, curr->m_row);
        exec_join(curr->m_next);
      }
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_inner_join_x_1 (std::shared_ptr<item> curr)
  {
    auto prev = (curr->m_opt_prev == nullptr ? curr->m_prev : curr->m_opt_prev);
    auto fgn = prev->tree()->get_schema()->get_foreign(curr->m_fgn_name);

    std::string key;
    for (auto col_name : fgn.col_name_list())
      key += prev->m_row.value_at(col_name);

    curr->m_niter = curr->tree()->find(curr->m_ndx_name, key);

    if (curr->m_niter != curr->tree()->end(curr->m_ndx_name)) {
      curr->m_row = *curr->m_niter;

      set_column_value(curr->m_on_cond, prev->m_row, curr->m_row);

      std::string res;
      exec_condition(res, curr->m_on_cond);

      if (res == "true") {
        for (auto it : m_expr_list) set_column_value(it, curr->m_row);
        set_column_value(m_where, curr->m_row);
        exec_join(curr->m_next);
      }
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_inner_join_0_0 (std::shared_ptr<item> curr)
  {
    auto prev = curr->m_prev;
    for (; prev->m_diter != prev->tree()->end(); ++prev->m_diter)
    {
      for (auto it : m_expr_list) set_column_value_null(it);

      prev->m_row = *prev->m_diter;

      for (curr->m_diter = curr->tree()->begin();
          curr->m_diter != curr->tree()->end();
          ++curr->m_diter)
      {
        yield();

        curr->m_row = *curr->m_diter;

        set_column_value(curr->m_on_cond, prev->m_row, curr->m_row);

        std::string res {"false"};
        exec_condition(res, curr->m_on_cond);

        if (res == "true") {
          for (auto it : m_expr_list)
            set_column_value(it, prev->m_row, curr->m_row);
          set_column_value(m_where, prev->m_row, curr->m_row);
          exec_join(curr->m_next);
        }
      }
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_inner_join_0_1 (std::shared_ptr<item> curr)
  {
    auto prev = curr->m_prev;
    auto fgn = prev->tree()->get_schema()->get_foreign(curr->m_fgn_name);
    for (; prev->m_diter != prev->tree()->end(); ++prev->m_diter)
    {
      yield();

      for (auto it : m_expr_list) set_column_value_null(it);

      prev->m_row = *prev->m_diter;

      std::string key;
      for (auto col_name : fgn.col_name_list())
        key += prev->m_row.value_at(col_name);

      curr->m_niter = curr->tree()->find(curr->m_ndx_name, key);

      std::string res {"false"};
      if (curr->m_niter != curr->tree()->end(curr->m_ndx_name)) {
        curr->m_row = *curr->m_niter;

        set_column_value(curr->m_on_cond, prev->m_row, curr->m_row);

        exec_condition(res, curr->m_on_cond);
      }

      if (res == "true") {
        for (auto it : m_expr_list)
          set_column_value(it, prev->m_row, curr->m_row);
        set_column_value(m_where, prev->m_row, curr->m_row);
        exec_join(curr->m_next);
      }
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_join (std::shared_ptr<item> curr) {
    switch (curr->m_action) {
      case TYPE_CRJOIN:
        break;
      case TYPE_INJOIN:
        exec_inner_join(curr);
        break;
      case TYPE_LTJOIN:
        exec_left_join(curr);
        break;
      case TYPE_OUTPUT:
        output_expr_list();
        break;
      case TYPE_WHERE:
        exec_where(curr);
        break;
      default:;
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_left_join (std::shared_ptr<item> curr) {
    auto prev = curr->m_prev;

    if (prev->m_prev != nullptr) {
      if (curr->m_ndx_name.empty()) exec_left_join_x_0(curr);
      else exec_left_join_x_1(curr);
    } else {
      if (curr->m_ndx_name.empty()) exec_left_join_0_0(curr);
      else exec_left_join_0_1(curr);
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_left_join_x_0 (std::shared_ptr<item> curr)
  {
    auto prev = (curr->m_opt_prev == nullptr ? curr->m_prev : curr->m_opt_prev);
    for (curr->m_diter = curr->tree()->begin();
        curr->m_diter != curr->tree()->end();
        ++curr->m_diter)
    {
      yield();

      curr->m_row = *curr->m_diter;

      set_column_value(curr->m_on_cond, prev->m_row, curr->m_row);

      std::string res;
      exec_condition(res, curr->m_on_cond);

      if (res == "true") {
        for (auto it : m_expr_list) set_column_value(it, curr->m_row);
        set_column_value(m_where, curr->m_row);
        exec_join(curr->m_next);
      }
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_left_join_x_1 (std::shared_ptr<item> curr)
  {
    auto prev = (curr->m_opt_prev == nullptr ? curr->m_prev : curr->m_opt_prev);
    auto fgn = prev->tree()->get_schema()->get_foreign(curr->m_fgn_name);

    std::string key;
    for (auto col_name : fgn.col_name_list())
      key += prev->m_row.value_at(col_name);

    curr->m_niter = curr->tree()->find(curr->m_ndx_name, key);

    if (curr->m_niter != curr->tree()->end(curr->m_ndx_name)) {
      curr->m_row = *curr->m_niter;

      set_column_value(curr->m_on_cond, prev->m_row, curr->m_row);

      std::string res;
      exec_condition(res, curr->m_on_cond);

      if (res == "true") {
        for (auto it : m_expr_list) set_column_value(it, curr->m_row);
        set_column_value(m_where, curr->m_row);
        exec_join(curr->m_next);
      }
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_left_join_0_0 (std::shared_ptr<item> curr)
  {
    auto prev = curr->m_prev;
    for (; prev->m_diter != prev->tree()->end(); ++prev->m_diter)
    {
      m_did_output = false;

      for (auto it : m_expr_list) set_column_value_null(it);

      prev->m_row = *prev->m_diter;

      for (curr->m_diter = curr->tree()->begin();
          curr->m_diter != curr->tree()->end();
          ++curr->m_diter)
      {
        yield();

        curr->m_row = *curr->m_diter;

        set_column_value(curr->m_on_cond, prev->m_row, curr->m_row);

        std::string res;
        exec_condition(res, curr->m_on_cond);

        if (res == "true") {
          for (auto it : m_expr_list)
            set_column_value(it, prev->m_row, curr->m_row);
          set_column_value(m_where, prev->m_row, curr->m_row);
          exec_join(curr->m_next);
        }
      }

      if (!m_did_output && !m_have_inner_join) {
        for (auto it : m_expr_list)
          set_column_value(it, prev->m_row);
        exec_join(m_tail);
      }
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_left_join_0_1 (std::shared_ptr<item> curr)
  {
    auto prev = curr->m_prev;
    auto fgn = prev->tree()->get_schema()->get_foreign(curr->m_fgn_name);

    for (; prev->m_diter != prev->tree()->end(); ++prev->m_diter)
    {
      yield();

      m_did_output = false;

      for (auto it : m_expr_list) set_column_value_null(it);

      prev->m_row = *prev->m_diter;

      std::string key;
      for (auto col_name : fgn.col_name_list())
        key += prev->m_row.value_at(col_name);

      curr->m_niter = curr->tree()->find(curr->m_ndx_name, key);

      if (curr->m_niter != curr->tree()->end(curr->m_ndx_name)) {
        curr->m_row = *curr->m_niter;

        set_column_value(curr->m_on_cond, prev->m_row, curr->m_row);

        std::string res;
        exec_condition(res, curr->m_on_cond);

        if (res == "true") {
          for (auto it : m_expr_list)
            set_column_value(it, prev->m_row, curr->m_row);
          set_column_value(m_where, prev->m_row, curr->m_row);
          exec_join(curr->m_next);
        }
      }

      if (!m_did_output && !m_have_inner_join) {
        for (auto it : m_expr_list)
          set_column_value(it, prev->m_row);
        exec_join(m_tail);
      }
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_mod (std::string& res, std::shared_ptr<item> oper)
  {
    try {
      std::string l_res;
      std::string r_res;
      exec_condition(l_res, oper->m_left);
      if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
      exec_condition(r_res, oper->m_right);
      if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
      float l_value = std::stof(l_res);
      float r_value = std::stof(r_res);
      float value = std::fmod(l_value, r_value);
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(2) << value;
      res = oss.str();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::exec_mul (std::string& res, std::shared_ptr<item> oper)
  {
    try {
      std::string l_res;
      std::string r_res;
      exec_condition(l_res, oper->m_left);
      if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
      exec_condition(r_res, oper->m_right);
      if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
      float l_value;
      float r_value;
      if (is_number(l_value, l_res) && is_number(r_value, r_res)) {
        std::ostringstream oss;
        oss << (l_value * r_value);
        res = oss.str();
      } else throw std::runtime_error{
        eno::get_msg(eno::ERRNO_MALFORMED_MULTIPLY)};
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::exec_or (std::string& res, std::shared_ptr<item> oper)
  {
    std::string l_res;
    std::string r_res;
    exec_condition(l_res, oper->m_left);
    if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
    bool l_value;
    std::istringstream(l_res) >> std::boolalpha >> l_value;
    if (l_value) { res = l_res; return; }
    exec_condition(r_res, oper->m_right);
    if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
    bool r_value;
    std::istringstream(r_res) >> std::boolalpha >> r_value;
    if (r_value) { res = r_res; return; }
    res = red::VM_FALSE;
  }

  /****************************************************************************
   *
   */
  void vm::exec_query () {
    auto end = m_head->tree()->end();
    for (m_head->m_diter = m_head->tree()->begin(); m_head->m_diter != end;
        ++m_head->m_diter)
    {
      for (auto it : m_expr_list) set_column_value_null(it);

      m_head->m_row = *m_head->m_diter;

      set_column_value(m_where, m_head->m_row);

      for (auto it : m_expr_list) set_column_value(it, m_head->m_row);

      exec_join(m_head->m_next);
    }
  }

  /****************************************************************************
   *
   */
  void vm::exec_sub (std::string& res, std::shared_ptr<item> oper)
  {
    try {
      std::string l_res;
      std::string r_res;
      exec_condition(l_res, oper->m_left);
      if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
      exec_condition(r_res, oper->m_right);
      if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
      float l_value;
      float r_value;
      if (is_number(l_value, l_res) && is_number(r_value, r_res)) {
        std::ostringstream oss;
        oss << (l_value - r_value);
        res = oss.str();
      } else throw std::runtime_error{
        eno::get_msg(eno::ERRNO_MALFORMED_SUBTRACT)};
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::exec_where (std::shared_ptr<item> it) {
    std::string res{"true"};
    exec_condition(res, it->m_on_cond);
    if (res == "true") exec_join(it->m_next);
  }

  /****************************************************************************
   *
   */
  void vm::exec_xor (std::string& res, std::shared_ptr<item> oper)
  {
    std::string l_res;
    std::string r_res;
    exec_condition(l_res, oper->m_left);
    if (l_res == NULL_VALUE) { res = NULL_VALUE; return; }
    exec_condition(r_res, oper->m_right);
    if (r_res == NULL_VALUE) { res = NULL_VALUE; return; }
    bool l_value;
    bool r_value;
    std::istringstream(l_res) >> std::boolalpha >> l_value;
    std::istringstream(r_res) >> std::boolalpha >> r_value;
    std::ostringstream oss;
    oss << std::boolalpha
      << ((l_value || r_value) && not (l_value && r_value));
    res = oss.str();
  }

  /****************************************************************************
   *
   */
  void vm::fsm_flush () {
    m_fsm->set_opcode(m_opcode == comm::OP_NULL ? comm::OP_RSLT_FIN : m_opcode);
    m_fsm->flush();
  }

  /****************************************************************************
   *
   */
  void vm::get_column_name (
      std::vector<std::pair<std::string, std::string> >& col_val_list,
      std::shared_ptr<schema::schema> sch)
  {
    for (auto column : sch->get_column_list())
      col_val_list.push_back(std::make_pair(column.name(), ""));
  }

  /****************************************************************************
   *
   */
  void vm::get_index_name (std::shared_ptr<item> prev,
      std::shared_ptr<item> next, std::shared_ptr<item> on_cond)
  {
    std::vector<std::pair<std::shared_ptr<item>,
      std::shared_ptr<item> > > column_list;

    make_column_list(column_list, prev, next, on_cond);

    std::string l_fgn_name;
    for (auto fgn : prev->tree()->get_schema()->get_foreign_list()) {
      if (fgn.db_name() == next->tree()->db_name()
          && fgn.tbl_name() == next->tree()->tbl_name())
      {
        bool match {true};
        for (auto col_name : fgn.col_name_list()) {
          auto iter = std::find_if(column_list.begin(), column_list.end(),
              [&](auto p){
              return p.second->m_db_name == next->tree()->db_name()
              && p.second->m_tbl_name == next->tree()->tbl_name()
              && p.second->m_col_name == col_name;});
          if (iter == column_list.end()) { match = false; break; }
        }
        if (match) { l_fgn_name = fgn.name(); break; }
      }
    }

    if (not l_fgn_name.empty()) {
      next->m_ndx_name = schema::PK_NDX;
      next->m_fgn_name = l_fgn_name;
      next->m_diter = next->tree()->end();
    }
  }
  
  /****************************************************************************
   *
   */
  std::string vm::get_varchar (int32_t offset) {
    int32_t size = static_cast<int32_t>(m_data.get_int16(offset));
    return m_data.get_varchar(size);
  }

  /****************************************************************************
   *
   */
  void vm::grant () {
    try {
      // pop item off stack
      auto e1 = m_stack.top(); m_stack.pop();
      // get count
      int32_t count = e1->value();
      // process privileges
      std::vector<std::string> priv_list;
      while (count-- > 0) {
        // pop item off stack
        auto e2 = m_stack.top(); m_stack.pop();
        // get privilege
        int32_t offset2 = e2->value();
        // push privilege onto list
        priv_list.push_back(get_varchar(offset2 + 1));
      }
      // pop item off stack
      e1 = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e1->value();
      // get db_name
      std::string db_name = get_varchar(offset + 1);
      // pop item off stack
      e1 = m_stack.top(); m_stack.pop();
      // get offset
      offset = e1->value();
      // get tbl_name
      std::string tbl_name = get_varchar(offset + 1);
      // pop item off stack
      e1 = m_stack.top(); m_stack.pop();
      // get offset
      offset = e1->value();
      // get username
      std::string username = get_varchar(offset + 1);
      // pop item off stack
      e1 = m_stack.top(); m_stack.pop();
      // get offset
      offset = e1->value();
      // get hostname
      std::string hostname = get_varchar(offset + 1);
      // verify privilege
      verify_privilege(db_name, tbl_name, dbms::GRANT_PRIV);
      // grant privilege(s)
      if ((db_name.empty() || db_name == dbms::ASTERISK)
          && tbl_name == dbms::ASTERISK)
        grant_global(hostname, username, priv_list);
      else
        grant_local(hostname, username, db_name, tbl_name, priv_list);
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed
        << std::setw(4) << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void populate_all_privilege_list(std::vector<std::string>& all_priv_list,
      std::vector<std::string> const& priv_list,
      std::vector<std::string> const& base_all_list)
  {
    for (auto col_name : base_all_list) {
      auto iter = std::find(priv_list.begin(), priv_list.end(), col_name);
      if (iter == priv_list.end() && col_name != dbms::GRANT_PRIV)
        all_priv_list.push_back(col_name);
    }
  }

  /****************************************************************************
   *
   */
  void normalize_privilege_list (std::vector<std::string>& priv_list,
      bool global)
  {
    // find an ALL_PRIV in privilege-list
    auto iter = std::find(priv_list.begin(), priv_list.end(),
        dbms::ALL_PRIV);
    // return, if no ALL_PRIV found
    if (iter == priv_list.end()) return;
    // purge all ALL_PRIVs from privilege-list
    do {
      priv_list.erase(iter);
      iter = std::find(priv_list.begin(), priv_list.end(),
          dbms::ALL_PRIV);
    } while (iter != priv_list.end());
    // construct all-privilege-list with all privileges (except GRANT_PRIV)
    std::vector<std::string> all_priv_list;
    if (global)
      populate_all_privilege_list(all_priv_list, priv_list,
          dbms::GLOBAL_PRIVILEGE_LIST);
    else
      populate_all_privilege_list(all_priv_list, priv_list,
          dbms::LOCAL_PRIVILEGE_LIST);
    // add all-privilege-list to privilege-list
    for (auto col_name : all_priv_list) priv_list.push_back(col_name);
  }

  /****************************************************************************
   *
   */
  void vm::grant_global (std::string const& hostname,
      std::string const& username, std::vector<std::string>& priv_list)
  {
    // verify username@hostname
    // construct schema-name
    std::ostringstream sch_name;
    sch_name << dbms::ETC_DIR << '.' << dbms::CRED_TBL;
    // get block-cache object
    auto blk_cache = m_dbms->get_block_cache(sch_name.str());
    // construct tree object
    auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
    // unique-lock tree
    the_tree->set_unique_lock();
    // construct row object
    auto rw = the_tree->make_row();
    // populate row
    rw.append(0, hostname);
    rw.append(1, username);
    // make primary-key
    std::string key = rw.make_key(schema::PK_NDX);
    // find corresponding row, if exists
    auto iter1 = the_tree->find(schema::PK_NDX, key);
    // row does not exist
    if (iter1 == the_tree->end(schema::PK_NDX))
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_NAME)};
    // normalize privilege list
    normalize_privilege_list(priv_list, true);
    // get original row
    auto org_row = *iter1;
    // populate row
    rw.init(org_row);
    rw.append(0, org_row.value_at(0));
    rw.append(1, org_row.value_at(1));
    rw.append(2, org_row.value_at(2));
    // enable privileges
    size_t pos {3}; // skip hostname, username and password
    for (auto iter2 = rw.get_schema()->get_column_list().begin() + pos;
        iter2 != rw.get_schema()->get_column_list().end(); ++iter2)
    {
      auto iter3 = std::find_if(priv_list.begin(), priv_list.end(),
          [&](auto col_name){return(col_name == (*iter2).name());});
      rw.append(pos, (iter3 == priv_list.end() ? org_row.value_at(pos) : "Y"));
      ++pos;
    }
    // construct emplace updater
    auto emdate = tree::emplace{the_tree};
    // do update
    emdate.update(rw);
    // flush tree
    m_dbms->flush(the_tree);
  }

  /****************************************************************************
   *
   */
  void vm::grant_local (std::string const& hostname,
      std::string const& username, std::string const& db_name,
      std::string const& tbl_name, std::vector<std::string>& priv_list)
  {
    // verify username@hostname
    {
      // construct schema-name
      std::ostringstream sch_name;
      sch_name << dbms::ETC_DIR << '.' << dbms::CRED_TBL;
      // get block-cache object
      auto blk_cache = m_dbms->get_block_cache(sch_name.str());
      // construct tree object
      auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
      // unique-lock tree
      the_tree->set_unique_lock();
      // construct row object
      auto rw1 = the_tree->make_row();
      // populate row
      rw1.append(0, hostname);
      rw1.append(1, username);
      // make primary-key
      std::string key = rw1.make_key(schema::PK_NDX);
      // find corresponding row, if exists
      auto iter1 = the_tree->find(schema::PK_NDX, key);
      // row does not exist
      if (iter1 == the_tree->end(schema::PK_NDX))
        throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_NAME)};
    }
    // normalize privilege list
    normalize_privilege_list(priv_list, false);
    // construct schema-name
    std::ostringstream sch_name;
    sch_name << dbms::ETC_DIR << '.' << dbms::PRIV_TBL;
    // get block-cache object
    auto blk_cache = m_dbms->get_block_cache(sch_name.str());
    // construct tree object
    auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
    // unique-lock tree
    the_tree->set_unique_lock();
    // construct row object
    auto rw = the_tree->make_row();
    // populate row
    rw.append(0, hostname);
    rw.append(1, username);
    rw.append(2, (db_name.empty() ? m_fsm->get_info().db_name() : db_name));
    rw.append(3, tbl_name);
    // make primary-key
    std::string key = rw.make_key(schema::PK_NDX);
    // find corresponding row, if exists
    auto iter1 = the_tree->find(schema::PK_NDX, key);
    // row does not exist
    if (iter1 == the_tree->end(schema::PK_NDX)) {
      size_t pos {4}; // skip hostname, username, db_name, tbl_name
      for (auto iter2 = rw.get_schema()->get_column_list().begin() + pos;
          iter2 != rw.get_schema()->get_column_list().end(); ++iter2)
      {
        auto iter3 = std::find_if(priv_list.begin(), priv_list.end(),
            [&](auto col_name){return(col_name == (*iter2).name());});
        rw.append(pos, (iter3 == priv_list.end() ? "N" : "Y"));
        ++pos;
      }
      // insert new row
      the_tree->insert(rw);
    } else {  // row does exist
      // get original row
      auto org_row = *iter1;
      // populate row
      rw.init(org_row);
      rw.append(0, org_row.value_at(0));
      rw.append(1, org_row.value_at(1));
      rw.append(2, org_row.value_at(2));
      rw.append(3, org_row.value_at(3));
      size_t pos {4}; // skip hostname, username, db_name, tbl_name
      for (auto iter2 = rw.get_schema()->get_column_list().begin() + pos;
          iter2 != rw.get_schema()->get_column_list().end(); ++iter2)
      {
        auto iter3 = std::find_if(priv_list.begin(), priv_list.end(),
            [&](auto col_name){return(col_name == (*iter2).name());});
        rw.append(pos, (iter3 == priv_list.end()
              ? org_row.value_at(pos) : "Y"));
        ++pos;
      }
      // construct emplace updater
      auto emdate = tree::emplace{the_tree};
      // do update
      emdate.update(rw);
    }
    // flush tree
    m_dbms->flush(the_tree);
  }

  /****************************************************************************
   *
   */
  void vm::inner_join () {
    if (m_stack.top()->m_type != red::TYPE_ON)
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_JOIN)};
    // pop on/using condition (item) off stack
    auto on_cond = m_stack.top(); m_stack.pop();
    // compact on condition
    compact_condition(on_cond);
    // if no join exists pop tree (item) off stack
    if (m_head == nullptr) {
      m_head = m_stack.top(); m_stack.pop();
      m_tail = m_head;
    }
    // pop tree (item) off stack
    m_tail->m_next = m_stack.top(); m_stack.pop();
    m_tail->m_next->m_prev = m_tail;
    m_tail = m_tail->m_next;
    m_tail->m_action = TYPE_INJOIN;
    m_tail->m_on_cond = on_cond;
  }

  /****************************************************************************
   * INSERT statement.
   */
  void vm::insert () {
    try {
      // get the tree
      auto the_tree = m_tree_list.back()->tree();
      // verify privilege
      verify_privilege(the_tree->db_name(), the_tree->tbl_name(),
          dbms::INSERT_PRIV);
      // load foreign-key tree(s)
      std::vector<std::pair<std::string,
        std::shared_ptr<tree::tree> > > fgn_tree_list;
      load_foreign_tree (fgn_tree_list,
          the_tree->get_schema()->get_foreign_list());
      // construct <column, value> list
      std::vector<std::pair<std::string, std::string> > col_val_list;
      // load column names
      load_column_name (the_tree->get_schema(), col_val_list);
      // get column names
      if (col_val_list.empty())
        get_column_name(col_val_list, the_tree->get_schema());
      // make a row
      row rw = the_tree->make_row();
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get count of value sets
      int32_t count = e->value();
      size_t row_count {0};
      // load sets of values
      while (count-- > 0 && !m_stack.empty()) {
        // load a set of values
        load_value_list(col_val_list);
        // verify values
        verify_value_list(the_tree, col_val_list);
        // construct complete value list
        std::vector<std::pair<std::string, std::string> > full_col_val_list;
        make_value_list(full_col_val_list, the_tree, col_val_list);
        // load values into row
        load_row(rw, full_col_val_list);
        // verify foreign key
        verify_foreign_key (the_tree->get_schema()->get_foreign_list(),
            fgn_tree_list, rw);
        // verify unique key(s)
        if (verify_unique_key(the_tree, rw)) {
          // insert row into table
          the_tree->insert(rw);
          ++row_count;
        }
        // clear row
        rw.clear();
      }
      // flush tree
      m_dbms->flush(the_tree);
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, " << row_count << " rows affected (" << std::fixed
        << std::setw(4) << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  bool vm::is_number (float& res, std::string const& text) {
    bool success {true};
    size_t pos {0L};
    try { res = std::stof(text, &pos); } catch (...) { success = false; }
    return (success && (pos == text.size()));
  }

  /****************************************************************************
   *
   */
  void vm::left_join () {
    if (m_stack.top()->m_type != red::TYPE_ON)
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_JOIN)};
    // pop on/using condition (item) off stack
    auto on_cond = m_stack.top(); m_stack.pop();
    // compact on condition
    compact_condition(on_cond);
    // if no join exists pop tree (item) off stack
    if (m_head == nullptr) {
      m_head = m_stack.top(); m_stack.pop();
      m_tail = m_head;
    }
    // pop tree (item) off stack
    m_tail->m_next = m_stack.top(); m_stack.pop();
    m_tail->m_next->m_prev = m_tail;
    m_tail = m_tail->m_next;
    m_tail->m_action = TYPE_LTJOIN;
    m_tail->m_on_cond = on_cond;
  }

  /****************************************************************************
   *
   */
  void vm::load_column_name (std::shared_ptr<schema::schema> sch,
      std::vector<std::pair<std::string, std::string> >& col_val_list)
  {
    // pop item off stack
    auto e1 = m_stack.top(); m_stack.pop();
    // get count of column names
    int32_t count = e1->value();
    // get column names
    while (count-- > 0) {
      // pop item off stack
      auto e2 = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset2 = e2->value();
      // get table name
      std::string col_name = get_varchar(offset2 + 1);
      // verify column name
      try {
        sch->get_column(col_name);
      } catch (...) {
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_COLUMN_LIST)};
      }
      // add column name to list
      col_val_list.push_back(std::make_pair(col_name, ""));
    }
  }

  /****************************************************************************
   *
   */
  bool vm::get_data (
      std::vector<std::pair<std::string, std::string> >& value_list,
      std::string const& line)
  {
    try {
      data_lexer lxr {line};
      size_t i {0L};
      for (; i < value_list.size(); ++i)
      {
        auto tok = lxr.next_token();
        if (tok.type() == data_lexer::EOI) return false;
        value_list[i].second = tok.text();
      }
      auto tok = lxr.next_token();
      // check for incorrect number of values; either too few or too many
      if (i != value_list.size()
          || tok.type() != static_cast<int32_t>(data_lexer::EOI)) return false;
      // made it
      return true;
    } catch (...) { return false; }
  }

  /****************************************************************************
   *
   */
  void vm::load_data () {
    int32_t const IGNORE  = 1;
    int32_t const REPLACE = 2;
    try {
      // get the tree
      auto the_tree = m_tree_list.back()->tree();
      // verify privilege
      verify_privilege(the_tree->db_name(), the_tree->tbl_name(),
          dbms::INSERT_PRIV);
      // load foreign-key tree(s)
      std::vector<std::pair<std::string,
        std::shared_ptr<tree::tree> > > fgn_tree_list;
      load_foreign_tree (fgn_tree_list,
          the_tree->get_schema()->get_foreign_list());
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get infile name
      std::string infile = get_varchar(offset + 1);
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      //TODO: IGNORE/REPLACE feature is not currently supported
      int32_t flag = e->value();
      switch (flag) {
        case IGNORE: case REPLACE: break;
        default: flag = IGNORE;
      }
      // construct infile path
      std::ostringstream path;
      path << dbms::SRC_DIR << '/' << infile;
      // open infile
      std::ifstream ifs{path.str()};
      // is infile open
      if (not ifs.is_open())
        throw std::runtime_error{eno::get_msg(eno::ERRNO_INFILE_ERROR)};
      // get row
      auto rw = the_tree->make_row();
      // process infile
      size_t row_count {0};
      while (true) {
        std::string line;
        std::getline(ifs, line);
        if (not ifs) break;
        std::vector<std::pair<std::string, std::string> > col_val_list;
        get_column_name(col_val_list, the_tree->get_schema());
        if (not get_data(col_val_list, line))
        {
          m_fsm->reset(red::comm::OP_RSLT_SYN);
          std::ostringstream oss;
          oss << eno::get_msg(eno::ERRNO_MALFORMED_VALUE_LIST) << ": "
            << line << '\n';
          m_fsm->put(oss.str());
          m_fsm->flush();
          continue;
        }
        // load values into row
        load_row(rw, col_val_list);
        // verify foreign key
        verify_foreign_key (the_tree->get_schema()->get_foreign_list(),
            fgn_tree_list, rw);
        // verify unique key(s)
        if (not verify_unique_key(the_tree, rw))
        {
          m_fsm->reset(red::comm::OP_RSLT_SYN);
          std::ostringstream oss;
          oss << eno::get_msg(eno::ERRNO_DUPLICATE_KEY) << ": " << line << '\n';
          m_fsm->put(oss.str());
          m_fsm->flush();
          continue;
        }
        // insert row into table
        the_tree->insert(rw);
        // clear row
        rw.clear();
        ++row_count;
        if ((row_count % 64L) == 0L) {
          m_fsm->reset(comm::OP_RSLT_SYN, comm::TYPE_TXT);
          m_fsm->put(1L, &DOT);
          m_fsm->flush();
        }
        yield();
      }
      // flush tree
      m_dbms->flush(the_tree);
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, " << row_count << " rows affected (" << std::fixed
        << std::setw(4) << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::load_foreign_tree (
      std::vector<std::pair<std::string,
        std::shared_ptr<tree::tree> > >& fgn_tree_list,
      std::vector<schema::foreign> const& fgn_list)
  {
    for (auto fgn : fgn_list) {
      // construct block-cache name
      std::ostringstream bc_name;
      bc_name << fgn.db_name() << '.' << fgn.tbl_name();
      auto block_cache = m_dbms->get_block_cache(bc_name.str());
      if (block_cache == nullptr)
        throw std::runtime_error{eno::get_msg(eno::ERRNO_DBMS_ERROR)};
      // construct tree object
      auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{block_cache}};
      // unique-lock tree
      the_tree->set_unique_lock();
      // emplace foreign-key name and tree onto list
      fgn_tree_list.emplace_back(fgn.name(), the_tree);
    }
  }

  /****************************************************************************
   *
   */
  void vm::load_row (row& rw,
      std::vector<std::pair<std::string, std::string> > const& col_val_list)
  {
    for (size_t i = 0; i < col_val_list.size(); ++i)
      rw.append(i, col_val_list[ i ].second);
  }

  /****************************************************************************
   *
   */
  void vm::load_value_list (
      std::vector<std::pair<std::string, std::string> >& col_val_list)
  {
    // pop item off stack
    auto e1 = m_stack.top(); m_stack.pop();
    // get count
    size_t count = static_cast<size_t>(e1->value());
    // check count against size of list
    if (col_val_list.size() != count)
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_VALUE_LIST)};
    // load values
    for (size_t i = 0; i < count; ++i) {
      // pop item off stack
      auto e2 = m_stack.top(); m_stack.pop();
      // load value into list
      col_val_list[ i ].second = e2->m_value;
    }
  }

  /****************************************************************************
   *
   */
  void vm::logic_and () {
    item* it = new item;
    it->m_type = TYPE_AND;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   *
   */
  void vm::logic_not () {
    item* it = new item;
    it->m_type = TYPE_NOT;
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   *
   */
  void vm::logic_or () {
    item* it = new item;
    it->m_type = TYPE_OR;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   * SET portion of UPDATE statement.
   * Prep set.
   */
  void vm::make_assignment () {
    // construct (assignment) item
    auto it = std::shared_ptr<item>{new item};
    it->m_type = TYPE_ASSIGN;
    // get (column) item
    it->m_left = m_stack.top(); m_stack.pop();
    // verify column is not auto-increment
    if ((it->m_left->col_flag() & schema::FLAG_AUTO_INC)
        == schema::FLAG_AUTO_INC
        || (it->m_left->col_flag() & schema::FLAG_PRIMARY)
        == schema::FLAG_PRIMARY)
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_UPDATE_ERROR)};
    // get (expression) item
    it->m_right = m_stack.top(); m_stack.pop();
    // push assignment onto list
    m_assignment_list.push_front(std::make_pair(it->m_left->m_col_name, it));
  }

  /****************************************************************************
   * Asterisk expression (*) portion of SELECT statement.
   * Prep asterisk expression.
   */
  void vm::make_asterisk_expression () {
    // pop item off stack
    auto e1 = m_stack.top(); m_stack.pop();
    // get offset
    int32_t offset1 = e1->value();
    // get database name
    std::string db_name = get_varchar(offset1 + 1);
    // pop item off stack
    e1 = m_stack.top(); m_stack.pop();
    // get offset
    offset1 = e1->value();
    // get table name
    std::string tbl_name = get_varchar(offset1 + 1);
    // find corresponding tree(s)
    std::vector<std::shared_ptr<item> > tree_list;
    if (not db_name.empty() && not tbl_name.empty()) {
      auto iter = std::find_if(m_tree_list.begin(), m_tree_list.end(),
          [&](auto it){return (it->m_db_name == db_name
              && (it->m_tbl_name == tbl_name || it->m_alias == tbl_name));});
      if (iter == m_tree_list.end())
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_COLUMN_EXPR)};
      tree_list.push_back(*iter);
    } else if (not tbl_name.empty()) {
      auto iter = std::find_if(m_tree_list.begin(), m_tree_list.end(),
          [&](auto it){return (it->m_db_name == m_fsm->get_info().db_name()
            && (it->m_tbl_name == tbl_name || it->m_alias == tbl_name));});
      if (iter == m_tree_list.end())
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_COLUMN_EXPR)};
      tree_list.push_back(*iter);
    } else {
      auto iter = m_tree_list.end();
      for (--iter; iter != m_tree_list.begin(); --iter)
        tree_list.push_back(*iter);
      tree_list.push_back(*iter);
    }
    // remove title from title-list
    m_title_stack.pop();
    // construct and push (column) items onto column stack
    std::stack<std::shared_ptr<item> > col_item_stack;
    for (auto tree_it : tree_list) {
      auto the_sch = tree_it->tree()->get_schema();

      for (auto col : the_sch->get_column_list()) {
        auto col_it = std::shared_ptr<item> {new item};

        col_it->m_type = TYPE_COLUMN;
        col_it->m_quote = (col.type() == schema::TYPE_CHAR
            || col.type() == schema::TYPE_VARCHAR);
        col_it->m_alias = tree_it->m_alias;
        col_it->m_db_name = the_sch->get_header().db_name();
        col_it->m_tbl_name = the_sch->get_header().tbl_name();
        col_it->m_col_name = col.name();
        col_it->m_parent = tree_it;
        col_item_stack.push(col_it);

        // construct title
        std::ostringstream title;
        if (not col_it->m_alias.empty()) title << col_it->m_alias << '.';
        title << col_it->m_col_name;
        // assign title to (column) item
        col_it->m_title = title.str();
      }
    }
    // push (column) items onto main stack (in reverse order)
    while (not col_item_stack.empty()) {
      auto col_it = col_item_stack.top();
      m_stack.push(col_it);
      // also push (column) title onto title stack for make_expression_list()
      m_title_stack.push(col_it->m_title);
      col_item_stack.pop();
    }
  }

  /****************************************************************************
   * Column definition portion of CREATE TABLE statement.
   * Make a column object and add column object to working schema.
   */
  void vm::make_column_definition () {
    // pop item off stack
    auto e1 = m_stack.top(); m_stack.pop();
    // get offset of column name
    int32_t offset1 = e1->value();
    // get column name
    std::string name = get_varchar(offset1 + 1);
    // pop item off stack
    e1 = m_stack.top(); m_stack.pop();
    // get column type
    int32_t type = e1->value();
    // initialze size, precision, emum_list
    int32_t size {0};
    int32_t precision {0};
    std::vector<std::string> enum_list;
    // process enumerations
    if (type == schema::TYPE_ENUM)
      make_enum_list(enum_list, size);
    else {
      // pop item off stack
      auto e2 = m_stack.top(); m_stack.pop();
      // get column size
      size = e2->value();
      // get column precision
      if (type == schema::TYPE_FLOAT) {
        // pop item off stack
        auto e3 = m_stack.top(); m_stack.pop();
        // get column precision
        precision = e3->value();
      }
    }
    // pop item off stack
    e1 = m_stack.top(); m_stack.pop();
    // get column flag
    int32_t flag = e1->value();
    // push back schema::column object
    switch (type) {
      case schema::TYPE_CHAR:
      case schema::TYPE_BINARY:
      case schema::TYPE_INT:
      case schema::TYPE_VARCHAR:
        m_wrk_sch->m_column.push_back(schema::column {
            flag, size, type, name});
        break;
      case schema::TYPE_DATE:
      case schema::TYPE_DATETIME:
      case schema::TYPE_TIMESTAMP:
        m_wrk_sch->m_column.push_back(schema::column {
            flag, type, name});
        break;
      case schema::TYPE_ENUM:
        m_wrk_sch->m_column.push_back(schema::column {
            flag, size, type, name, enum_list});
        break;
      case schema::TYPE_FLOAT:
        m_wrk_sch->m_column.push_back( schema::column {
            flag, precision, size, type, name});
        break;
      default:;
    }
  }

  /****************************************************************************
   *
   */
  void vm::make_column_expression () {
    // pop item off stack
    auto e1 = m_stack.top(); m_stack.pop();
    // get offset
    int32_t offset = e1->value();
    // get database name
    std::string db_name = get_varchar(offset + 1);
    if (db_name.empty()) db_name = m_fsm->get_info().db_name();
    // pop item off stack
    e1 = m_stack.top(); m_stack.pop();
    // get offset
    offset = e1->value();
    // get table name/alias
    std::string tbl_name = get_varchar(offset + 1);
    // pop item off stack
    e1 = m_stack.top(); m_stack.pop();
    // get offset
    offset = e1->value();
    // get column name
    std::string col_name = get_varchar(offset + 1);
    // find corresponding (tree) item
    std::shared_ptr<item> it_tree;
    if (not db_name.empty() && not tbl_name.empty()) {
      auto iter2 = std::find_if(m_tree_list.begin(), m_tree_list.end(),
          [&](auto it){return (it->m_db_name == db_name
              && (it->m_tbl_name == tbl_name || it->m_alias == tbl_name));});
      if (iter2 == m_tree_list.end())
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_COLUMN_EXPR)};
      it_tree = *iter2;
    } else {
      it_tree = m_tree_list.back();
    }
    // find corresponding column
    auto iter
      = std::find_if(it_tree->tree()->get_schema()->get_column_list().begin(),
          it_tree->tree()->get_schema()->get_column_list().end(),
          [&](auto col){return (col.name() == col_name);});
    if (iter == it_tree->tree()->get_schema()->get_column_list().end())
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_COLUMN_EXPR)};
    // construct an item for column
    auto it = new item;
    it->m_type = TYPE_COLUMN;
    it->m_quote = ((*iter).type() == schema::TYPE_CHAR
        || (*iter).type() == schema::TYPE_VARCHAR);
    it->m_db_name = it_tree->tree()->db_name();
    it->m_tbl_name = it_tree->tree()->tbl_name();
    it->m_col_name = col_name;
    it->m_parent = it_tree;
    m_stack.emplace(it);  // push onto stack
  }

  /****************************************************************************
   *
   */
  void vm::make_column_list (std::vector<std::pair<std::shared_ptr<item>,
      std::shared_ptr<item> > >& column_list,
      std::shared_ptr<item> l_item, std::shared_ptr<item> r_item,
      std::shared_ptr<item> on_cond)
  {
    switch (on_cond->m_type) {
      case TYPE_CMPEQ:
      case TYPE_CMPGE:
      case TYPE_CMPGT:
      case TYPE_CMPLE:
      case TYPE_CMPLT:
      case TYPE_CMPNE: {
          if (on_cond->m_left->m_type == TYPE_COLUMN
              && on_cond->m_right->m_type == TYPE_COLUMN)
          {
            if (on_cond->m_left->m_parent == l_item
                && on_cond->m_right->m_parent == r_item)
            {
              auto p = std::make_pair(on_cond->m_left, on_cond->m_right);
              column_list.push_back(p);
            } else if (on_cond->m_left->m_parent == r_item
                && on_cond->m_right->m_parent == l_item)
            {
              auto p = std::make_pair(on_cond->m_right, on_cond->m_left);
              column_list.push_back(p);
            }
          }
        break;
      }
      case TYPE_AND:
      case TYPE_OR:
        make_column_list(column_list, l_item, r_item, on_cond->m_left);
        make_column_list(column_list, l_item, r_item, on_cond->m_right);
        break;
      case TYPE_ON:
      case TYPE_WHERE:
        make_column_list(column_list, l_item, r_item, on_cond->m_on_cond);
        break;
      case TYPE_XOR:
        // TODO: future?
      default:;
    }
  }

  /****************************************************************************
   * Interpret ENUM (name, ... name) declaration.
   * Populate an enumeration list and record size of largest enumeration
   * string.
   */
  void vm::make_enum_list (std::vector<std::string>& enum_list, int32_t& size)
  {
    // pop item off stack
    auto e1 = m_stack.top(); m_stack.pop();
    // get enumeration-count
    int32_t count = e1->value();
    // process enumerations
    while (count-- > 0) {
      // pop item off stack
      auto e2 = m_stack.top(); m_stack.pop();
      // get offset of enumeration
      int32_t offset = e2->value();
      // get enumeration
      enum_list.push_back(get_varchar(offset + 1));
    }
    // set size equal to size of longest enumeration
    size = 0;
    for (std::string e : enum_list)
      if (e.size() > static_cast<size_t>(size))
        size = static_cast<int32_t>(e.size());
  }

  /****************************************************************************
   *
   */
  void vm::make_expression () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get type
      int32_t type = static_cast<int32_t>(m_data.get(offset));
      // construct item
      auto it = new item;
      // get value
      it->m_value = get_varchar(offset + 1);
      // get output size
      if (it->m_out_size < static_cast<int32_t>(it->m_value.size()))
        it->m_out_size = static_cast<int32_t>(it->m_value.size());
      // test type, localize type and test value
      switch (type) {
        case schema::TYPE_INT:
          it->m_type = TYPE_INT;
          std::stoi(it->m_value);
          break;
        case schema::TYPE_FLOAT:
          it->m_type = TYPE_FLOAT;
          std::stof(it->m_value);
          break;
        case schema::TYPE_VARCHAR:
          it->m_type = TYPE_VARCHAR;
          it->m_quote = true;
          break;
        default:
          throw std::runtime_error{eno::get_msg(
              eno::ERRNO_MALFORMED_EXPR)};
      }
      // push item onto stack
      m_stack.emplace(it);
    } catch (...) {
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_EXPR)};
    }
  }

  /****************************************************************************
   *
   */
  void unescape (std::string& value) {
    std::string temp;
    char last_ch {'\0'};
    for (auto ch : value) {
      if (last_ch == '\\') {
        if (ch == '\"') temp += ch;
        else { temp += last_ch; temp += ch; }
      } else if (ch != '\\') temp += ch;
      last_ch = ch;
    }
    value = temp;
  }

  /****************************************************************************
   *
   */
  void vm::make_expression_header () {
    // pop item off stack
    auto e = m_stack.top(); m_stack.pop();
    // get offset
    int32_t offset = e->value();
    // get title
    std::string title = get_varchar(offset + 1);
    // remove escape characters
    unescape(title);
    // push title onto list
    m_title_stack.push(title);
  }

  /****************************************************************************
   *
   */
  void vm::make_expression_list () {
    while (!m_stack.empty()) {
      if (m_stack.top()->m_type != TYPE_TREE) {
        auto it = m_stack.top();
        compact_condition(it);
        if (not m_title_stack.empty()) {
          it->m_title = m_title_stack.top(); m_title_stack.pop();
          if (static_cast<size_t>(it->m_out_size) < it->m_title.size())
            it->m_out_size = static_cast<int32_t>(it->m_title.size());
        }
        m_expr_list.push_back(it);
      }
      m_stack.pop();
    }

    if (m_expr_list.empty())
      throw std::runtime_error{eno::get_msg(eno::ERRNO_MALFORMED_EXPR_LIST)};
  }

  /****************************************************************************
   * Interpret FOREIGN KEY (column_name, ..., column_name) declaration.
   * Construct a foreign-key object and add object to working schema. All for-
   * eign-key reference objects are created after working schema is finalized
   * in function create_table().
   */
  void vm::make_foreign_key () {
    // pop item off stack
    auto e1 = m_stack.top(); m_stack.pop();
    // get foreign-key-count
    int32_t count = e1->value();
    // get foreign-key column names of working schema
    std::vector<std::string> fk_col_name_list;
    while (count-- > 0) {
      // pop item off stack
      auto e2 = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset2 = e2->value();
      // get foreign-key column name
      std::string col_name = get_varchar(offset2 + 1);
      // add column name to list
      fk_col_name_list.push_back(col_name);
    }
    // pop item off stack
    e1 = m_stack.top(); m_stack.pop();
    // get offset
    int32_t offset1 = e1->value();
    // get foreign database name
    std::string db_name = get_varchar(offset1 + 1);
    if (db_name.empty()) db_name = m_fsm->get_info().db_name();
    // pop item off stack
    e1 = m_stack.top(); m_stack.pop();
    // get offset
    offset1 = e1->value();
    // get foreign table name
    std::string tbl_name = get_varchar(offset1 + 1);
    // pop item off stack
    e1 = m_stack.top(); m_stack.pop();
    // get foreign-key-count
    count = e1->value();
    // get primary-key column names of foreign table
    std::vector<std::string> pk_col_name_list;
    while (count-- > 0) {
      // pop item off stack
      auto e2 = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset2 = e2->value();
      // get foreign-key column name
      std::string col_name = get_varchar(offset2 + 1);
      // add column name to list
      pk_col_name_list.push_back(col_name);
    }
    // throw exception if number of foreign-key column names differs from
    // number of primary-key column names
    if (fk_col_name_list.size() != pk_col_name_list.size())
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_FOREIGN_KEY)};
    // throw exception if any foreign-key column names do not match actual
    // column names in working schema
    for (auto name : fk_col_name_list) {
      auto iter = std::find_if(m_wrk_sch->m_column.begin(),
          m_wrk_sch->m_column.end(),
          [&](auto column){ return column.name() == name; });
      if (iter == m_wrk_sch->m_column.end())
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_FOREIGN_KEY)};
    }
    // construct schema name of foreign table
    std::ostringstream fgn_sch_name;
    fgn_sch_name << db_name << '.' << tbl_name;
    // get schema of foreign table
    auto fgn_sch = m_dbms->get_schema_mgr()->get_schema(fgn_sch_name.str());
    // throw exception if foreign table not found
    if (fgn_sch == nullptr)
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_FOREIGN_KEY)};
    // throw exception if number of primary-key column names differs from
    // number of foreign table primary-key columns
    if (pk_col_name_list.size() != fgn_sch->get_index(schema::PK_NDX)
        .col_name_list().size())
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_FOREIGN_KEY)};
    // throw exception if any name in primary-key column name list differs from
    // the names of the foreign table primary-key columns
    for (size_t i = 0; i < pk_col_name_list.size(); ++i)
      if (pk_col_name_list[ i ] != fgn_sch->get_index(schema::PK_NDX)
          .col_name_list()[ i ])
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_FOREIGN_KEY)};
    // construct foreign-key name
    auto fk_name = schema::name<schema::foreign>{schema::FGN_,
      m_wrk_sch->m_foreign}.make();
    // construct foreign-key object and add foreign-key object to working
    // schema
    m_wrk_sch->m_foreign.push_back(schema::foreign {fk_name, db_name,
        tbl_name, pk_col_name_list});
  }

  /****************************************************************************
   * PRIMARY KEY (column_name, ..., column_name) declaration.
   * Construct a primary-key object and add object to working schema.
   */
  void vm::make_primary_key () {
    // throw exception if primary-key index already exists
    for (auto ndx : m_wrk_sch->m_index)
      if (ndx.name() == schema::PK_NDX)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MULTIPLE_PRIMARY_KEY)};
    // pop item off stack
    auto e1 = m_stack.top(); m_stack.pop();
    // get key-count
    int32_t count = e1->value();
    // process key list
    std::vector<std::string> col_name_list;
    while (count-- > 0) {
      // pop item off stack
      auto e2 = m_stack.top(); m_stack.pop();
      // get key-count
      int32_t offset2 = e2->value();
      // get column name
      auto col_name = get_varchar(offset2 + 1);
      // add column name to list
      col_name_list.push_back(col_name);
    }
    // throw exception if any names in col_name_list are wrong
    // throw exception if a corresponding column is already flagged as PRIMARY
    // KEY or UNIQUE KEY
    for (auto name : col_name_list) {
      auto iter = std::find_if(m_wrk_sch->m_column.begin(),
          m_wrk_sch->m_column.end(),
          [&](auto clmn){return name == clmn.name();});
      if (iter == m_wrk_sch->m_column.end()
          || (iter->flag() & schema::FLAG_PRIMARY)
          == schema::FLAG_PRIMARY // unlikely
          || (iter->flag() & schema::FLAG_UNIQUE)
          == schema::FLAG_UNIQUE) // possible
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_PRIMARY_KEY)};
    }
    // flag columns and calculate size of primary-key
    int32_t pk_size {0};
    constexpr int32_t flag = (schema::FLAG_PRIMARY
        | schema::FLAG_NOT_NULL | schema::FLAG_UNIQUE);
    for (size_t i = 0; i < col_name_list.size(); ++i) {
      m_wrk_sch->m_column[ i ].flag(m_wrk_sch->m_column[ i ].flag() | flag);
      pk_size += m_wrk_sch->m_column[ i ].size();
    }
    // construct binary tree calculator
    tree::bcalc bc {pk_size};
    // push primary-key index onto index list
    m_wrk_sch->m_index.push_back(schema::index {flag, bc.order(), pk_size,
        schema::PK_NDX, col_name_list});
  }

  /****************************************************************************
   * SELECT statement: make query
   */
  void vm::make_query () {
    if (m_tree_list.size() == 1) {
      m_head = m_tree_list.back();
      m_tail = m_head;
    }
    // set have-inner-join flag
    m_have_inner_join = false;
    auto curr = m_head->m_next;
    while (curr != nullptr && !m_have_inner_join) {
      m_have_inner_join = (curr->m_action == TYPE_INJOIN);
      curr = curr->m_next;
    }
    // get last right-join
    curr = m_tail;
    while (curr != nullptr && curr->m_action != TYPE_RTJOIN)
      curr = curr->m_prev;
    // flip-flop-n-away
    if (curr != nullptr) {
      std::vector<std::shared_ptr<item> > item_list;
      auto stub = curr->m_next;
      // push items onto list in reverse order
      auto each = curr;
      while (each != nullptr) {
        item_list.push_back(each);
        each = each->m_prev;
      }
      // reverse prev and next pointers
      for (size_t i = 1; i < item_list.size(); ++i) {
        item_list[ i - 1 ]->m_next = item_list[ i ];
        item_list[ i ]->m_prev = item_list[ i - 1 ];
      }
      // move each on-condition object one item right
      // move each action type one item right
      for (auto i = item_list.size() - 1; i >= 1; --i) {
        item_list[ i ]->m_action = item_list[ i - 1 ]->m_action;
        if (item_list[ i ]->m_action == TYPE_LTJOIN)
          item_list[ i ]->m_action = TYPE_INJOIN;
        if (item_list[ i ]->m_action == TYPE_RTJOIN)
          item_list[ i ]->m_action = TYPE_LTJOIN;
        item_list[ i ]->m_on_cond = item_list[ i - 1 ]->m_on_cond;
      }
      // clear action of lead item
      item_list.front()->m_action = TYPE_NULL;
      // clear on-condition of lead item
      item_list.front()->m_on_cond = nullptr;
      // set edge pointers
      item_list.front()->m_prev = nullptr;
      item_list.back()->m_next = stub;
      // set optimized prev-pointer and update prev-pointer for stub item
      if (stub != nullptr) {
        stub->m_opt_prev = stub->m_prev;
        stub->m_prev = item_list.back();
      }
      // push stub items onto list
      each = stub;
      while (each != nullptr) {
        item_list.push_back(each);
        each = each->m_next;
      }
      // set head and tail pointers
      m_head = item_list.front();
      m_tail = item_list.back();
    } else {
      auto prev = m_tail;
      auto next = m_head;
      int32_t count {0};
      while (next != nullptr && prev != nullptr && next != prev) {
        prev = prev->m_prev;
        next = next->m_next;
        ++count;
      }
      if (next != nullptr && prev != nullptr && count > 1) {
        std::vector<std::shared_ptr<item> > item_list;
        auto stub = next->m_next;
        // push items onto list in reverse order
        auto each = next;
        while (each != nullptr) {
          item_list.push_back(each);
          each = each->m_prev;
        }
        // reverse prev and next pointers
        for (size_t i = 1; i < item_list.size(); ++i) {
          item_list[ i - 1 ]->m_next = item_list[ i ];
          item_list[ i ]->m_prev = item_list[ i - 1 ];
        }
        // move each on-condition object one item right
        // move each action type one item right
        for (auto i = item_list.size() - 1; i >= 1; --i) {
          item_list[ i ]->m_action = item_list[ i - 1 ]->m_action;
          if (item_list[ i ]->m_action == TYPE_LTJOIN)
            item_list[ i ]->m_action = TYPE_INJOIN;
          item_list[ i ]->m_on_cond = item_list[ i - 1 ]->m_on_cond;
        }
        // clear action of lead item
        item_list.front()->m_action = TYPE_NULL;
        // clear on-condition of lead item
        item_list.front()->m_on_cond = nullptr;
        // set edge pointers
        item_list.front()->m_prev = nullptr;
        item_list.back()->m_next = stub;
        // set optimized prev-pointer and update prev-pointer for stub item
        if (stub != nullptr) {
          stub->m_opt_prev = stub->m_prev;
          stub->m_prev = item_list.back();
        }
        // push stub items onto list
        each = stub;
        while (each != nullptr) {
          item_list.push_back(each);
          each = each->m_next;
        }
        // set head and tail pointers
        m_head = item_list.front();
        m_tail = item_list.back();
      }
    }
    // if possible, configure a join index for each join
    curr = m_head;
    while (curr != nullptr && curr->m_next != nullptr) {
      if (curr->m_on_cond != nullptr)
        get_index_name(curr, curr->m_next, curr->m_next->m_on_cond);
      curr = curr->m_next;
    }
    // append where-condition
    if (m_where != nullptr) {
      m_tail->m_next = m_where;
      m_where->m_prev = m_tail;
      m_tail = m_where;
    }
    // append output item
    auto output = std::shared_ptr<item>{new item};
    output->m_type = TYPE_OUTPUT;
    output->m_action = TYPE_OUTPUT;
    output->m_prev = m_tail;
    m_tail->m_next = output;
    m_tail = output;
  }

  /****************************************************************************
   * Construct foreign-key reference objects and add objects to corresponding
   * foreign schemas.
   */
  void vm::make_reference (std::string const& db_name,
      std::string const& tbl_name,
      std::vector<schema::foreign> const& fgn_list)
  {
    for (auto fgn : fgn_list) {
      // construct foreign schema name
      std::ostringstream fgn_sch_name;
      fgn_sch_name << fgn.db_name() << '.' << fgn.tbl_name();
      // get foreign schema
      auto fgn_sch = m_dbms->get_schema_mgr()->get_schema(fgn_sch_name.str());
      if (fgn_sch == nullptr)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_DBMS_ERROR)};
      // populate foreign schema with referenced-by objects
      for (size_t i = 0; i < fgn.col_name_list().size(); ++i) {
        // construct referenced-by name
        std::string rb_name = schema::name<schema::reference>{schema::REF_,
          fgn_sch->column_at(i).reference_list()}.make();
        // construct referenced-by object
        schema::reference ref_by {rb_name, db_name, tbl_name, fgn.name()};
        // add referenced-by object to foreign schema
        fgn_sch->column_at(i).reference_list().push_back(ref_by);
      }
      m_dbms->get_schema_mgr()->save_schema(fgn_sch_name.str());
    }
  }

  /****************************************************************************
   *
   */
  void vm::make_work_column_list (std::vector<schema::column>& column_list,
      std::shared_ptr<item> l_item, std::shared_ptr<item> r_item)
  {
    int32_t id {0};
    for (auto it : m_expr_list) {
      switch (it->m_type) {
        case TYPE_ADD:
        case TYPE_DIV:
        case TYPE_FLOAT:
        case TYPE_INT:
        case TYPE_MOD:
        case TYPE_MUL:
        case TYPE_SUB: {
          std::ostringstream name;
          name << "col_" << std::setw(2) << std::setfill('0') << id++;
          column_list.push_back(schema::column {0, 2, 12,
              schema::TYPE_FLOAT, name.str()});
          break;
        }
        case TYPE_COLUMN:
          if (l_item->tree()->db_name() == it->m_db_name
              && l_item->tree()->tbl_name() == it->m_tbl_name)
          {
            auto col = l_item->tree()->get_schema()
              ->get_column(it->m_col_name);
            column_list.push_back(col);
          } else if (r_item->tree()->db_name() == it->m_db_name
              && r_item->tree()->tbl_name() == it->m_tbl_name)
          {
            auto col = r_item->tree()->get_schema()
              ->get_column(it->m_col_name);
            column_list.push_back(col);
          }
          break;
        case TYPE_BOOLEAN:
        case TYPE_CMPEQ:
        case TYPE_CMPGE:
        case TYPE_CMPGT:
        case TYPE_CMPLE:
        case TYPE_CMPLT:
        case TYPE_CMPNE:
        case TYPE_AND:
        case TYPE_NOT:
        case TYPE_OR: {
          std::ostringstream name;
          name << "col_" << std::setw(2) << std::setfill('0') << id++;
          column_list.push_back(schema::column {0, 0, 1,
              schema::TYPE_BOOLEAN, name.str()});
          break;
        }
        default:;
      }
    }
  }

  /****************************************************************************
   * UNIQUE KEY portion of CREATE TABLE statement.
   */
  void vm::make_unique_key () {
    // pop item off stack
    auto e1 = m_stack.top(); m_stack.pop();
    // get key-count
    int32_t count = e1->value();
    // process key list
    std::vector<std::string> col_name_list;
    while (count-- > 0) {
      // pop item off stack
      auto e2 = m_stack.top(); m_stack.pop();
      // get key-count
      int32_t offset2 = e2->value();
      // get column name
      auto col_name = get_varchar(offset2 + 1);
      // add column name to list
      col_name_list.push_back(col_name);
    }
    // throw exception if an identical index already exists
    for (auto ndx : m_wrk_sch->m_index) {
      bool match {true};
      for (auto name1 : col_name_list) {
        auto iter = std::find_if(ndx.col_name_list().begin(),
            ndx.col_name_list().end(),
            [&](auto name2){return name1 == name2;});
        if (iter == ndx.col_name_list().end()) { match = false; break; }
      }
      if (match)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_UNIQUE_KEY)};
    }
/*
    // throw exception if no corresponding column can be found or a correspond-
    // ing column is already flagged as FOREIGN KEY, PRIMARY KEY or UNIQUE key
    for (auto name : col_name_list) {
      auto iter = std::find_if(m_wrk_sch->m_column.begin(),
          m_wrk_sch->m_column.end(),
          [&] (auto column) { return column.name() == name; });
      if (iter == m_wrk_sch->m_column.end()
          || (iter->flag() & schema::FLAG_FOREIGN)
          == schema::FLAG_FOREIGN
          || (iter->flag() & schema::FLAG_PRIMARY)
          == schema::FLAG_PRIMARY
          || (iter->flag() & schema::FLAG_UNIQUE)
          == schema::FLAG_UNIQUE)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_UNIQUE_KEY)};
    }
*/
    // flag corresponding columns as unique and calculate unique-key size
    int32_t uk_size {0};
    constexpr int32_t flag {schema::FLAG_UNIQUE};
    for (auto name : col_name_list) {
      auto iter = std::find_if(m_wrk_sch->m_column.begin(),
          m_wrk_sch->m_column.end(),
          [&] (auto column) { return column.name() == name; });
      if (iter != m_wrk_sch->m_column.end()) {
        iter->flag(iter->flag() | flag);
        uk_size += iter->size();
      }
    }
    // construct binary tree calculator
    tree::bcalc bc {uk_size};
    // construct name for unique-key index
    auto uk_name = schema::name<schema::index>{schema::NDX_,
      m_wrk_sch->m_index}.make();
    // push unique-key index onto index list
    m_wrk_sch->m_index.push_back(schema::index {flag, bc.order(), uk_size,
        uk_name, col_name_list});
  }

  /****************************************************************************
   *
  void vm::make_update_key (std::string& key,
      std::shared_ptr<schema::schema> sch,
      std::vector<std::pair<std::string, std::string> >& col_val_list)
  {
    std::string ndx_name;
    size_t column_count {0};
    for (auto& ndx : sch->get_index_list()) {
      bool match {true};
      for (auto& col_name : ndx.col_name_list()) {
        auto iter = std::find_if(col_val_list.begin(), col_val_list.end(),
            [&](auto& p){return(p.first == col_name);});
        if (iter == col_val_list.end()) { match = false; break; }
      }
      if (match && ndx.col_name_list().size() > column_count) {
        ndx_name = ndx.name();
        column_count = ndx.col_name_list().size();
      }
    }
    if (not ndx_name.empty()) {
      auto iter = std::find_if(sch->get_index_list().begin(),
          sch->get_index_list().end(),
          [&](auto& ndx){return(ndx.name() == ndx_name);});
      for (auto& col_name : (*iter).col_name_list()) {
        auto iter2 = std::find_if(col_val_list.begin(), col_val_list.end(),
            [&](auto& p){return(p.first == col_name);});
        auto col = sch->get_column(col_name);
        std::ostringstream value;
        if (col.type() == schema::TYPE_FLOAT) {
          value << std::right << std::setw(col.size()) << std::setfill('0')
            << std::to_string(std::stof((*iter2).second));
        } else if (col.type() == schema::TYPE_INT) {
          value << std::right << std::setw(col.size()) << std::setfill('0')
            << std::to_string(std::stoi((*iter2).second));
        } else 
          value << std::left << std::setw(col.size()) << std::setfill(' ')
            << (*iter2).second;
        key += value.str();
      }
    }
  }
   */

  /****************************************************************************
   * Execute USER() command; push hostname and username onto stack.
   */
  void vm::make_user () {
    auto it = new item;
    it->m_type = TYPE_VARCHAR;
    it->m_value = m_fsm->get_info().hostname();
    m_stack.emplace(it);
    it = new item;
    it->m_type = TYPE_VARCHAR;
    it->m_value = m_fsm->get_info().username();
    m_stack.emplace(it);
  }

  /****************************************************************************
   * VALUE list portion of INSERT statement.
   */
  void vm::make_value_list (
      std::vector<std::pair<std::string, std::string> >& full_col_val_list,
      std::shared_ptr<tree::tree> the_tree,
      std::vector<std::pair<std::string, std::string> >& col_val_list)
  {
    for (auto column : the_tree->get_schema()->get_column_list()) {
      auto iter = std::find_if(col_val_list.begin(), col_val_list.end(),
          [&] (auto p) { return (p.first == column.name()); });
      if (iter == col_val_list.end()) {
        full_col_val_list.push_back(std::make_pair(column.name(), ""));
      } else {
        full_col_val_list.push_back(std::make_pair(iter->first, iter->second));
      }
    }
  }

  /****************************************************************************
   * Construct a working schema object
   */
  void vm::make_working_schema () {
    m_wrk_sch = std::shared_ptr<work_schema>{new work_schema};
  }

  /****************************************************************************
   * Multiplication (*) definition.
   * Construct a multiplication (*) item and push onto stack.
   */
  void vm::mul () {
    item* it = new item;
    it->m_type = TYPE_MUL;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   *
   */
  void vm::negate () {
    auto it = m_stack.top(); m_stack.pop();
    it->m_negate = true;
    switch (it->m_type) {
      case red::TYPE_COLUMN:
        switch (it->col_type()) {
          case schema::TYPE_FLOAT:
          case schema::TYPE_INT:
            break;
          default:
            throw std::runtime_error{eno::get_msg(
                eno::ERRNO_MALFORMED_COLUMN_EXPR)};
        }
        break;
      case red::TYPE_FLOAT:
      case red::TYPE_INT:
        it->value(it->m_value);
        break;
      default:
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_EXPR)};
    }
    m_stack.push(it);
  }

  /****************************************************************************
   * ON condition portion of SELECT statement.
   * Prep on-condition.
   */
  void vm::on_cond () {
    auto oc_it = m_stack.top(); m_stack.pop();
    item* it = new item;
    it->m_type = TYPE_ON;
    it->m_on_cond = oc_it;
    m_stack.emplace(it);
    // decrement table-index
    --m_tbl_index;
  }

  /****************************************************************************
   * Table reference portion of ALTER statement.
  void vm::open_schema () {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get name of database
      std::string db_name = get_varchar(offset + 1);
      // if missing database name then get name of database in use
      if (db_name.empty()) db_name = m_fsm->get_info().db_name();
      // if no database name then throw exception
      if (db_name.empty())
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_TABLE_REF)};
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset
      offset = e->value();
      // get table name
      std::string tbl_name = get_varchar(offset + 1);
      // verify privilege
      verify_privilege(db_name, tbl_name, dbms::SELECT_PRIV);
      // construct schema name
      std::ostringstream sch_name;
      sch_name << db_name << '.' << tbl_name;
      // get schema
      m_schema = m_dbms->get_schema_mgr()->get_schema(sch_name.str());
      if (m_schema == nullptr)
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_TABLE_REF)};
  }
   */

  /****************************************************************************
   * Table reference portion of SELECT statement.
   */
  void vm::open_table () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e->value();
      // get name of database
      std::string db_name = get_varchar(offset + 1);
      // if missing database name then get name of database in use
      if (db_name.empty()) db_name = m_fsm->get_info().db_name();
      // if no database name then throw exception
      if (db_name.empty())
        throw std::runtime_error{eno::get_msg(eno::ERRNO_MALFORMED_TABLE_REF)};
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset
      offset = e->value();
      // get table name
      std::string tbl_name = get_varchar(offset + 1);
      // pop item off stack
      e = m_stack.top(); m_stack.pop();
      // get offset
      offset = e->value();
      // get alias
      std::string alias = get_varchar(offset + 1);
      // verify privilege
      verify_privilege(db_name, tbl_name, dbms::SELECT_PRIV);
      // construct block-cache name (schema-name)
      std::ostringstream bc_name;
      bc_name << db_name << '.' << tbl_name;
      // get block-cache
      auto blk_cache = m_dbms->get_block_cache(bc_name.str());
      // if the block-cache was not found throw exceptoin
      if (blk_cache == nullptr)
        throw std::runtime_error{eno::get_msg(eno::ERRNO_MALFORMED_TABLE_REF)};
      // construct tree object
      auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
      //# # # the_tree will be locked in shared_lock_tree or unique_lock_tree
      // construct item for tree
      auto rw = the_tree->make_row();
      auto it = std::shared_ptr<item>{new item {rw}};
      it->m_type = TYPE_TREE;
      it->m_alias = alias;
      it->m_db_name = db_name;
      it->m_tbl_name = tbl_name;
      it->m_diter = the_tree->begin();
      it->m_tree = the_tree;
      // push item onto stack and tree list
      m_stack.push(it);
      m_tree_list.push_back(m_stack.top());
      // increment table-index
      ++m_tbl_index;
    } catch(...) { throw; }
  }

  /****************************************************************************
   * ORDER BY portion of SELECT statement
   * Prep order-by.
   */
  void vm::order_by () {
    // pop item off stack
    auto e = m_stack.top(); m_stack.pop();
    // get count
    int32_t count = e->value();
    // pop order-by (column) item(s) off stack and push onto key-list
    while (count-- > 0) {
      auto it = m_stack.top(); m_stack.pop();
      it->m_action = TYPE_ORDERBY;
      m_key_list.push_back(it);
    }
    // construct new expression-list with items from key-list and
    // expression-list
    std::vector<std::shared_ptr<item> > expr_list;
    for (auto it : m_key_list) expr_list.push_back(it);
    for (auto it : m_expr_list) expr_list.push_back(it);
    // clear the expression-list
    m_expr_list.clear();
    // assign new expression list
    m_expr_list = expr_list;
  }

  /****************************************************************************
   * SELECT statement result.
   * Output boxed line of intermediate result of SELECT statement.
   */
  void vm::output_boxed_line (std::string& row)
  {
    std::string line;
    line = VB;
    line += SPC;
    auto iter = row.begin();
    for (size_t i = m_key_list.size(); i < m_expr_list.size(); ++i) {
      std::string value;
      for (; iter != row.end() && *iter != static_cast<char>(174); ++iter)
        value += *iter;
      if (value.empty()) value = "null";
      // break if iterator at end of row
      if (iter == row.end()) break;
      ++iter; // skip char(174)
      // get expression-type
      int32_t type = m_expr_list[i]->m_type;
      if (type == red::TYPE_COLUMN)
        type = m_expr_list[i]->col_type();
      // output expression-value
      switch (type) {
        case schema::TYPE_BOOLEAN:
        case red::TYPE_AND:
        case red::TYPE_CMPEQ:
        case red::TYPE_CMPGE:
        case red::TYPE_CMPGT:
        case red::TYPE_CMPLE:
        case red::TYPE_CMPLT:
        case red::TYPE_OR:
        case red::TYPE_XOR:
          {
            std::ostringstream oss;
            oss << std::boolalpha << std::left
              << std::setw(m_expr_list[i]->m_out_size)
              << std::setfill(SPC) << value;
            line += oss.str();
          }
          break;
        case schema::TYPE_CHAR:
        case schema::TYPE_DATE:
        case schema::TYPE_DATETIME:
        case schema::TYPE_ENUM:
        case schema::TYPE_TIMESTAMP:
        case schema::TYPE_VARCHAR:
        case red::TYPE_VARCHAR:
          {
            std::ostringstream oss;
            oss << std::left
              << std::setw(m_expr_list[i]->m_out_size)
              << std::setfill(SPC) << value;
            line += oss.str();
          }
          break;
        case schema::TYPE_BINARY:
          break;
        case schema::TYPE_FLOAT:
        case schema::TYPE_INT:
        case red::TYPE_ADD:
        case red::TYPE_DIV:
        case red::TYPE_FLOAT:
        case red::TYPE_INT:
        case red::TYPE_MUL:
        case red::TYPE_SUB:
          {
            std::ostringstream oss;
            oss << std::right << std::setw(m_expr_list[i]->m_out_size)
              << std::setfill(SPC) << value;
            line += oss.str();
          }
          break;
        default:;
      }
      line += SPC;
      line += VB;
      line += SPC;
    }

    line.pop_back();
    line += NL;
    m_fsm->put(line);
  }

  /****************************************************************************
   * SELECT statement result.
   * Output intermediate result as boxed result of SELECT statement.
   */
  size_t vm::output_boxed_result () {
    std::ostringstream title;
    title << VB;
    for (size_t i = m_key_list.size(); i < m_expr_list.size(); ++i) {
      title << SPC;
      title << std::left << std::setw(m_expr_list[i]->m_out_size)
        << std::setfill(' ') << m_expr_list[i]->m_title << SPC <<  VB;
    }
    title << NL;

    std::string barbwire {PS};
    for (size_t i = m_key_list.size(); i < m_expr_list.size(); ++i) {
      barbwire.append(m_expr_list[i]->m_out_size + 2, MS);
      barbwire += PS;
    }
    barbwire += NL;

    m_fsm->reset(red::comm::OP_RSLT_SYN, red::comm::TYPE_TXT);
    m_fsm->put(barbwire);
    m_fsm->put(title.str());
    m_fsm->put(barbwire);

    size_t row_count {0L};

    for (auto iter = m_output.begin(); iter != m_output.end(); ++iter)
    {
      output_boxed_line((*iter).second);
      ++row_count;
    }

    m_fsm->put(barbwire);

    return row_count;
  }

  /****************************************************************************
   * SELECT statement result.
   * Intermediate output of SELECT statement.
   */
  void vm::output_expr_list () {
    m_did_output = true;
    std::ostringstream row;
    for (size_t i = m_key_list.size(); i < m_expr_list.size(); ++i)
    {
      std::string res;
      exec_condition(res, m_expr_list[i]);
      switch (m_expr_list[i]->m_type) {
        case red::TYPE_ADD:
        case red::TYPE_DIV:
        case red::TYPE_FLOAT:
        case red::TYPE_MOD:
        case red::TYPE_MUL:
        case red::TYPE_SUB:
          {
            float value;
            std::istringstream(res) >> value;
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(m_expr_list[i]->m_out_prec)
              << value;
            res = oss.str();
          }
          break;
        default:;
      }

      if (m_expr_list[i]->m_out_size < static_cast<int32_t>(res.size()))
        m_expr_list[i]->m_out_size = static_cast<int32_t>(res.size());

      if (m_output_type == TYPE_FILE_OUTPUT) {
        if (i > 0) row << ", ";
        if (m_expr_list[i]->m_quote) row << '\"' << res << '\"';
        else row << res;
      } else row << res << static_cast<char>(174);
    }
    std::ostringstream key;
    if (m_key_list.empty())
      key << std::setw(20) << std::setfill('0') << m_output.size();
    else {
      for (size_t i = 0; i < m_key_list.size(); ++i)
      {
        switch (m_key_list[i]->col_type()) {
          case schema::TYPE_FLOAT:
            {
              float value;
              std::istringstream(m_key_list[i]->m_value) >> value;
              key << std::right << std::setfill('0')
                << std::setw(m_key_list[i]->col_size())
                << std::setprecision(m_key_list[i]->col_prec())
                << std::fixed << value;
            }
            break;
          case schema::TYPE_INT:
            {
              key << std::right << std::setfill('0')
                << std::setw(m_key_list[i]->col_size())
                << m_key_list[i]->m_value;
            }
            break;
          default:
            key << std::left << std::setfill(' ')
              << std::setw(m_key_list[i]->col_size())
              << m_key_list[i]->m_value;
        }
      }
    }
    m_output.insert({key.str(), row.str()});
  }

  /****************************************************************************
   *
   */
  size_t vm::output_result () {
    std::ofstream os;
    os.open(m_outfile, (os.trunc | os.out));
    if (not os.is_open())
      throw std::runtime_error{eno::get_msg(eno::ERRNO_OUTFILE_ERROR)};

    m_fsm->reset(red::comm::OP_RSLT_SYN, red::comm::TYPE_TXT);

    size_t row_count {0L};

    for (auto iter = m_output.begin(); iter != m_output.end(); ++iter)
    {
//      output_line(os, (*iter).second);
      os << (*iter).second << '\n';
      ++row_count;
    }

    os.flush();

    return row_count;
  }

  /****************************************************************************
   * SELECT statement result.
   * Output result of SELECT statement.
   */
  size_t vm::output_query_result () {
    size_t row_count {0L};
    m_fsm->reset();
    if (not m_output.empty()) {
      if (m_output_type == TYPE_FILE_OUTPUT) {
        row_count = output_result();
      } else {
        row_count = output_boxed_result();
      }
    }
    return row_count;
  }

  /****************************************************************************
   *
  bool get_column_value_pair (std::shared_ptr<item> it1,
      std::shared_ptr<item> it2,
      std::vector<std::pair<std::string, std::string> >& list)
  {
    if (it1 == nullptr || it2 == nullptr) return false;

    switch (it1->m_type) {
      case TYPE_COLUMN:
        list.emplace_back(it1->m_col_name, "");
        break;
      case TYPE_BOOLEAN:
      case TYPE_FLOAT:
      case TYPE_INT:
      case TYPE_VARCHAR:
        list.emplace_back("", it1->m_value);
        break;
      default: return false;
    }

    switch (it2->m_type) {
      case TYPE_COLUMN:
        list.back().first = it2->m_col_name;
        break;
      case TYPE_BOOLEAN:
      case TYPE_FLOAT:
      case TYPE_INT:
      case TYPE_VARCHAR:
        list.back().second = it2->m_value;
        break;
      default: { list.pop_back(); return false; }
    }

    return true;
  }
   */

  /****************************************************************************
   *
  void vm::parse_where (std::shared_ptr<item> it,
      std::vector<std::pair<std::string, std::string> >& list)
  {
    if (it == nullptr) return;
    switch (it->m_type) {
      case TYPE_CMPEQ:
      case TYPE_CMPGE:
      case TYPE_CMPGT:
      case TYPE_CMPLE:
      case TYPE_CMPLT:
      case TYPE_CMPNE:
      case TYPE_AND:
      case TYPE_OR:
      case TYPE_XOR:
        if (not get_column_value_pair(it->m_left, it->m_right, list)) {
          parse_where(it->m_left, list);
          parse_where(it->m_right, list);
        }
        break;
      case TYPE_WHERE:
        parse_where(it->m_on_cond, list);
        break;
      default:;
    }
  }
   */

  /****************************************************************************
   * Push a value onto the stack.
   */
  void vm::push () {
    item* it = new item;
    it->m_type  = TYPE_INT;
    int32_t value = m_code.get_int32();
    it->m_value = std::to_string(value);
    m_stack.emplace(it);
  }

  /****************************************************************************
   * Push an offset onto the stack
   */
  void vm::pusho () {
    item* it = new item;
    it->m_type  = TYPE_OFFSET;
    int32_t value = m_code.get_int32();
    it->m_value = std::to_string(value);
    m_stack.emplace(it);
  }

  /****************************************************************************
   *
   */
  void vm::revoke () {
    try {
      // pop item off stack
      auto e1 = m_stack.top(); m_stack.pop();
      // get count
      int32_t count = e1->value();
      // process privileges
      std::vector<std::string> priv_list;
      while (count-- > 0) {
        // pop item off stack
        auto e2 = m_stack.top(); m_stack.pop();
        // get privilege
        int32_t offset2 = e2->value();
        // push privilege onto list
        priv_list.push_back(get_varchar(offset2 + 1));
      }
      // pop item off stack
      e1 = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e1->value();
      // get db_name
      std::string db_name = get_varchar(offset + 1);
      // pop item off stack
      e1 = m_stack.top(); m_stack.pop();
      // get offset
      offset = e1->value();
      // get tbl_name
      std::string tbl_name = get_varchar(offset + 1);
      // pop item off stack
      e1 = m_stack.top(); m_stack.pop();
      // get offset
      offset = e1->value();
      // get username
      std::string username = get_varchar(offset + 1);
      // pop item off stack
      e1 = m_stack.top(); m_stack.pop();
      // get offset
      offset = e1->value();
      // get hostname
      std::string hostname = get_varchar(offset + 1);
      // verify privilege
      verify_privilege(db_name, tbl_name, dbms::GRANT_PRIV);
      // grant privilege(s)
      if ((db_name.empty() || db_name == dbms::ASTERISK)
          && tbl_name == dbms::ASTERISK)
        revoke_global(hostname, username, priv_list);
      else
        revoke_local(hostname, username, db_name, tbl_name, priv_list);
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, 0 rows affected (" << std::fixed
        << std::setw(4) << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::revoke_global (std::string const& hostname,
      std::string const& username, std::vector<std::string>& priv_list)
  {
    // verify username@hostname
    // construct schema-name
    std::ostringstream sch_name;
    sch_name << dbms::ETC_DIR << '.' << dbms::CRED_TBL;
    // get block-cache object
    auto blk_cache = m_dbms->get_block_cache(sch_name.str());
    // construct tree object
    auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
    // unique-lock tree
    the_tree->set_unique_lock();
    // construct row object
    auto rw = the_tree->make_row();
    // populate row
    rw.append(0, hostname);
    rw.append(1, username);
    // make primary-key
    std::string key = rw.make_key(schema::PK_NDX);
    // find corresponding row, if exists
    auto iter1 = the_tree->find(schema::PK_NDX, key);
    // row does not exist
    if (iter1 == the_tree->end(schema::PK_NDX))
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_NAME)};
    // normalize privilege list
    normalize_privilege_list(priv_list, true);
    // get original row
    auto org_row = *iter1;
    // populate row
    rw.init(org_row);
    rw.append(0, org_row.value_at(0));
    rw.append(1, org_row.value_at(1));
    rw.append(2, org_row.value_at(2));
    // enable privileges
    size_t pos {3}; // skip hostname, username and password
    for (auto iter2 = rw.get_schema()->get_column_list().begin() + pos;
        iter2 != rw.get_schema()->get_column_list().end(); ++iter2)
    {
      auto iter3 = std::find_if(priv_list.begin(), priv_list.end(),
          [&](auto col_name){return(col_name == (*iter2).name());});
      rw.append(pos, (iter3 == priv_list.end() ? org_row.value_at(pos) : "N"));
      ++pos;
    }
    // construct emplace updater
    auto emdate = tree::emplace{the_tree};
    // do update
    emdate.update(rw);
    // flush tree
    m_dbms->flush(the_tree);
  }

  /****************************************************************************
   *
   */
  void vm::revoke_local (std::string const& hostname,
      std::string const& username, std::string const& db_name,
      std::string const& tbl_name, std::vector<std::string>& priv_list)
  {
    // verify username@hostname
    {
      // construct schema-name
      std::ostringstream sch_name;
      sch_name << dbms::ETC_DIR << '.' << dbms::CRED_TBL;
      // get block-cache object
      auto blk_cache = m_dbms->get_block_cache(sch_name.str());
      // construct tree object
      auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
      // unique-lock tree
      the_tree->set_unique_lock();
      // construct row object
      auto rw1 = the_tree->make_row();
      // populate row
      rw1.append(0, hostname);
      rw1.append(1, username);
      // make primary-key
      std::string key = rw1.make_key(schema::PK_NDX);
      // find corresponding row, if exists
      auto iter1 = the_tree->find(schema::PK_NDX, key);
      // row does not exist
      if (iter1 == the_tree->end(schema::PK_NDX))
        throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_NAME)};
    }
    // normalize privilege list
    normalize_privilege_list(priv_list, false);
    // construct schema-name
    std::ostringstream sch_name;
    sch_name << dbms::ETC_DIR << '.' << dbms::PRIV_TBL;
    // get block-cache object
    auto blk_cache = m_dbms->get_block_cache(sch_name.str());
    // construct tree object
    auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
    // unique-lock tree
    the_tree->set_unique_lock();
    // construct row object
    auto rw = the_tree->make_row();
    // populate row
    rw.append(0, hostname);
    rw.append(1, username);
    rw.append(2, (db_name.empty() ? m_fsm->get_info().db_name() : db_name));
    rw.append(3, tbl_name);
    // make primary-key
    std::string key = rw.make_key(schema::PK_NDX);
    // find corresponding row, if exists
    auto iter1 = the_tree->find(schema::PK_NDX, key);
    // row does not exist
    if (iter1 == the_tree->end(schema::PK_NDX))
      throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_NAME)};
    // get original row
    auto org_row = *iter1;
    // populate row
    rw.init(org_row);
    rw.append(0, org_row.value_at(0));
    rw.append(1, org_row.value_at(1));
    rw.append(2, org_row.value_at(2));
    rw.append(3, org_row.value_at(3));
    size_t pos {4}; // skip hostname, username, db_name, tbl_name
    for (auto iter2 = rw.get_schema()->get_column_list().begin() + pos;
        iter2 != rw.get_schema()->get_column_list().end(); ++iter2)
    {
      auto iter3 = std::find_if(priv_list.begin(), priv_list.end(),
          [&](auto col_name){return(col_name == (*iter2).name());});
      rw.append(pos, (iter3 == priv_list.end()
            ? org_row.value_at(pos) : "N"));
      ++pos;
    }
    // construct emplace updater
    auto emdate = tree::emplace{the_tree};
    // do update
    emdate.update(rw);
    // flush tree
    m_dbms->flush(the_tree);
  }

  /****************************************************************************
   * RIGHT JOIN portion of SELECT statement.
   * Prep right join.
   */
  void vm::right_join () {
    if (m_stack.top()->m_type != red::TYPE_ON)
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_MALFORMED_JOIN)};
    // pop on/using condition (item) off stack
    auto on_cond = m_stack.top(); m_stack.pop();
    // compact on condition
    compact_condition(on_cond);
    // if no join exists pop tree (item) off stack
    if (m_head == nullptr) {
      m_head = m_stack.top(); m_stack.pop();
      m_tail = m_head;
    }
    // pop tree (item) off stack
    m_tail->m_next = m_stack.top(); m_stack.pop();
    m_tail->m_next->m_prev = m_tail;
    m_tail = m_tail->m_next;
    m_tail->m_action = TYPE_RTJOIN;
    m_tail->m_on_cond = on_cond;
  }

  /****************************************************************************
   * SELECT statement.
   */
  void vm::select () {
    try {
      make_query();
      if (m_tree_list.size() == 1) exec_query();
      else exec_join(m_head->m_next);
      auto row_count = output_query_result();
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << row_count << " rows in set (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      if (oss.str().size() > m_fsm->remaining()) m_fsm->flush();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; throw; }
  }

  /****************************************************************************
   *
   */
  void vm::set_column_value (std::shared_ptr<item> it, row& a_row)
  {
    if (it == nullptr) return;

    switch (it->m_type) {
      case TYPE_ASSIGN:
        set_column_value(it->m_right, a_row);
        break;
      case TYPE_COLUMN:
        if (a_row.db_name() == it->m_db_name
            && a_row.tbl_name() == it->m_tbl_name)
        {
          it->value(a_row.value_at(it->m_col_name));
        }
        break;
      case TYPE_ON:
      case TYPE_WHERE:
        set_column_value(it->m_on_cond, a_row);
        break;
      case TYPE_ADD:
      case TYPE_BW_AND:
      case TYPE_BW_OR:
      case TYPE_BW_XOR:
      case TYPE_CMPEQ:
      case TYPE_CMPGE:
      case TYPE_CMPGT:
      case TYPE_CMPLE:
      case TYPE_CMPLT:
      case TYPE_CMPNE:
      case TYPE_DIV:
      case TYPE_MOD:
      case TYPE_MUL:
      case TYPE_AND:
      case TYPE_OR:
      case TYPE_SUB:
      case TYPE_XOR:
        set_column_value(it->m_left, a_row);
        set_column_value(it->m_right, a_row);
        break;
      default:;
    }
  }

  /****************************************************************************
   *
   */
  void vm::set_column_value (std::shared_ptr<item> it,
      row& l_row, row& r_row)
  {
    if (it == nullptr) return;

    switch (it->m_type) {
      case TYPE_ASSIGN:
        set_column_value(it->m_right, l_row, r_row);
        break;
      case TYPE_COLUMN:
        if (l_row.db_name() == it->m_db_name
            && l_row.tbl_name() == it->m_tbl_name)
        {
          it->value(l_row.value_at(it->m_col_name));
        } else if (r_row.db_name() == it->m_db_name
            && r_row.tbl_name() == it->m_tbl_name)
        {
          it->value(r_row.value_at(it->m_col_name));
        }
        break;
      case TYPE_ON:
      case TYPE_WHERE:
        set_column_value(it->m_on_cond, l_row, r_row);
        break;
      case TYPE_ADD:
      case TYPE_BW_AND:
      case TYPE_BW_OR:
      case TYPE_BW_XOR:
      case TYPE_CMPEQ:
      case TYPE_CMPGE:
      case TYPE_CMPGT:
      case TYPE_CMPLE:
      case TYPE_CMPLT:
      case TYPE_CMPNE:
      case TYPE_DIV:
      case TYPE_MOD:
      case TYPE_MUL:
      case TYPE_AND:
      case TYPE_OR:
      case TYPE_SUB:
      case TYPE_XOR:
        set_column_value(it->m_left, l_row, r_row);
        set_column_value(it->m_right, l_row, r_row);
        break;
      default:;
    }
  }

  /****************************************************************************
   *
   */
  void vm::set_column_value_null (std::shared_ptr<item> it)
  {
    if (it == nullptr) return;

    switch (it->m_type) {
      case TYPE_COLUMN:
        it->m_value.clear();
        break;
      case TYPE_ON:
      case TYPE_WHERE:
        set_column_value_null(it->m_on_cond);
        break;
      case TYPE_ADD:
      case TYPE_BW_AND:
      case TYPE_BW_OR:
      case TYPE_BW_XOR:
      case TYPE_CMPEQ:
      case TYPE_CMPGE:
      case TYPE_CMPGT:
      case TYPE_CMPLE:
      case TYPE_CMPLT:
      case TYPE_CMPNE:
      case TYPE_DIV:
      case TYPE_MOD:
      case TYPE_MUL:
      case TYPE_AND:
      case TYPE_OR:
      case TYPE_SUB:
      case TYPE_XOR:
        set_column_value_null(it->m_left);
        set_column_value_null(it->m_right);
        break;
      default:;
    }
  }

  /****************************************************************************
   * INTO OUTFILE portion of SELECT statement.
   * Verify outfile; construct and record outfile path.
   */
  void vm::set_outfile () {
    try {
      // pop item off stack
      auto it = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = it->value();
      // get outfile
      std::string outfile = get_varchar(offset + 1);
      // throw if outfile empty
      if (outfile.empty())
        throw std::runtime_error{eno::get_msg(eno::ERRNO_OUTFILE_ERROR)};
      // throw if dot-dot exists
      char const DOTDOT [3] {".."};
      if (outfile.find(DOTDOT) != std::string::npos)
        throw std::runtime_error{eno::get_msg(eno::ERRNO_OUTFILE_ERROR)};
      // throw if non-valid characters exist
      auto iter = std::find_if(outfile.begin(), outfile.end(),
          [](auto ch){return(not(std::isalnum(ch) || ch == '_'
              || ch == '.'));});
      if (iter != outfile.end())
        throw std::runtime_error{eno::get_msg(eno::ERRNO_OUTFILE_ERROR)};
      // construct outfile path
      std::ostringstream path;
      path << dbms::SRC_DIR << '/' << outfile;
      // set output type
      m_output_type = TYPE_FILE_OUTPUT;
      // set outfile path
      m_outfile = path.str();
    } catch (...) { throw; }
  }

  /****************************************************************************
   * SHOW DATABASES: support routine
   */
  bool ignore_dir (char const* dir_name) {
    static std::vector<std::string> const ignore_list {
      {"."}, {".."}, dbms::TMP_DIR, dbms::LOG_DIR};
    for (auto ignore_dir : ignore_list)
      if (ignore_dir.compare(dir_name) == 0) return true;
    return false;
  }

  /****************************************************************************
   * SHOW DATABASES statement.
   */
  void vm::show_databases () {
    try {
      // verify privilege
      verify_privilege(dbms::ASTERISK, dbms::ASTERISK,
          dbms::SHOW_DB_PRIV);
      std::string const TITLE {"Database"};
      errno = 0;
      DIR* dir {nullptr};
      if ((dir = opendir(m_dbms->path().c_str())) == nullptr)
        throw std::runtime_error {eno::get_msg(
            eno::ERRNO_SYSTEM_ERROR, errno)};
      std::vector<std::string> dir_list;
      size_t dir_size = TITLE.size();
      size_t de_size = offsetof(struct dirent, d_name) + FILENAME_MAX + 1;
      struct dirent* de
        = reinterpret_cast<struct dirent*>(calloc(1, de_size)); 
      struct dirent* result {nullptr};
      int32_t errcode {0};
      while ((errcode = readdir_r(dir, de, &result)) == 0 && result != nullptr)
      {
        if (ignore_dir(de->d_name)) continue;
#ifdef _DIRENT_HAVE_D_TYPE
        if (de->d_type == DT_DIR)
          dir_list.emplace_back(de->d_name);
#else
        std::ostringstream dir_path;
        dir_path << m_dbms->path() << '/' << de->d_name;
        struct stat sb;
        if (stat(dir_path.str().c_str(), &sb) < 0)
          throw std::runtime_error {eno::get_msg(
              eno::ERRNO_SYSTEM_ERROR, errno)};
        if (S_ISDIR(sb.st_mode))
          dir_list.emplace_back(de->d_name);
#endif
        if (dir_size < dir_list.back().size())
          dir_size = dir_list.back().size();
      }
      // deallocate struct dirent
      free(de);
      // close directory
      closedir(dir);

      if (errcode != 0)
        throw std::runtime_error {eno::get_msg(
            eno::ERRNO_SYSTEM_ERROR, errcode)};
      // output table of databases
      std::ostringstream line;
      m_fsm->reset(red::comm::OP_RSLT_SYN);
      if (not dir_list.empty()) {
        std::string barbwire {PS};
        barbwire.append(dir_size + 2, MS);
        barbwire += PS;
        barbwire += NL;

        m_fsm->put(barbwire);

        line << VB << SPC << std::left << std::setw(static_cast<int>(dir_size))
          << std::setfill(' ') << TITLE << SPC << VB << '\n';

        m_fsm->put(line.str());
        m_fsm->put(barbwire);

        for (auto dir_name : dir_list) {
          line.str("");
          line << VB << SPC << std::left
            << std::setw(static_cast<int>(dir_size))
            << std::setfill(' ') << dir_name << SPC << VB << '\n';
          m_fsm->put(line.str());
        }

        m_fsm->put(barbwire);
      }

      size_t const FOOTER_SZ {40};
      if (m_fsm->remaining() < FOOTER_SZ) {
        m_fsm->flush();
        m_fsm->reset();
      }
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      line.str("");
      line << dir_list.size() << " rows in set (" << std::fixed << std::setw(4)
        << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->put(line.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   * SHOW TABLES: support routine
   */
  bool is_table (std::string const& f_name) {
    if (f_name.size() - 2 > dbms::TBL_NAME_MAX) return false;
    auto iter = f_name.end();
    --iter;
    if (*iter != 'd') return false;
    --iter;
    if (*iter != '.') return false;
    return true;
  }

  /****************************************************************************
   * SHOW TABLES statment.
   */
  void vm::show_tables () {
    try {
      std::string const TITLE {"Table"};
      if (m_fsm->get_info().db_name().empty())
        throw std::runtime_error {eno::get_msg(
            eno::ERRNO_NO_DATABASE_ERROR)};
      // verify privilege
      verify_privilege(m_fsm->get_info().db_name(), dbms::ASTERISK,
          dbms::SELECT_PRIV);
      // construct path to database
      std::ostringstream db_path;
      db_path << m_dbms->path() << '/' << m_fsm->get_info().db_name();
      errno = 0;
      DIR* dir {nullptr};
      if ((dir = opendir(db_path.str().c_str())) == nullptr)
        throw std::runtime_error {eno::get_msg(
            eno::ERRNO_SYSTEM_ERROR, errno)};
      std::vector<std::string> file_list;
      size_t file_size = TITLE.size();
      size_t de_size = offsetof(struct dirent, d_name) + FILENAME_MAX + 1;
      struct dirent* de
        = reinterpret_cast<struct dirent*>(calloc(1, de_size)); 
      struct dirent* result {nullptr};
      int32_t errcode {0};
      while ((errcode = readdir_r(dir, de, &result)) == 0 && result != nullptr)
      {
        std::string f_name {de->d_name};
#ifdef _DIRENT_HAVE_D_TYPE
        if (de->d_type == DT_REG && is_table(f_name)) {
          // remove ".d" (or whatever) suffix
          f_name.resize(f_name.size() - 2);
          // push f_name onto list
          file_list.push_back(f_name);
          // update file_size
          if (file_size < file_list.back().size())
            file_size = file_list.back().size();
        }
#else
        std::ostringstream dir_path;
        dir_path << db_path.str() << '/' << de->d_name;
        struct stat sb;
        if (stat(dir_path.str().c_str(), &sb) < 0)
          throw std::runtime_error {eno::get_msg(
              eno::ERRNO_SYSTEM_ERROR, errno)};
        if (S_ISREG(sb.st_mode) && is_table(f_name)) {
          // remove ".d" (or whatever) suffix
          f_name.resize(f_name.size() - 2);
          // push f_name onto list
          file_list.push_back(f_name);
          // update file_size
          if (file_size < file_list.back().size())
            file_size = file_list.back().size();
        }
#endif
      }
      // deallocate struct dirent
      free(de);
      // close directory
      closedir(dir);

      if (errcode != 0)
        throw std::runtime_error {eno::get_msg(
            eno::ERRNO_SYSTEM_ERROR, errcode)};
      // output table of tables
      std::ostringstream line;
      m_fsm->reset(red::comm::OP_RSLT_SYN);
      if (not file_list.empty()) {
        std::string barbwire {PS};
        barbwire.append(file_size + 2, MS);
        barbwire += PS;
        barbwire += NL;

        m_fsm->put(barbwire);

        line << VB << SPC << std::left
          << std::setw(static_cast<int>(file_size))
          << std::setfill(' ') << TITLE << SPC << VB << '\n';

        m_fsm->put(line.str());
        m_fsm->put(barbwire);

        for (auto f_name : file_list) {
          line.str("");
          line << VB << SPC << std::left
            << std::setw(static_cast<int>(file_size)) << std::setfill(' ')
            << f_name << SPC << VB << '\n';
          m_fsm->put(line.str());
        }

        m_fsm->put(barbwire);
      }

      size_t const FOOTER_SZ {40};
      if (m_fsm->remaining() < FOOTER_SZ) {
        m_fsm->flush();
        m_fsm->reset();
      }
      // stop time
      auto t1 = Time::now();
      // calculate elapsed time
      fsec fs = t1 - m_t0;
      line.str("");
      line << file_list.size() << " rows in set (" << std::fixed
        << std::setw(4) << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->put(line.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   * Interpret a subtraction (-) definition.
   * Construct an mathematical subtraction (-) item and push onto stack.
   */
  void vm::sub () {
    item* it = new item;
    it->m_type = TYPE_SUB;
    it->m_left = m_stack.top(); m_stack.pop();
    it->m_right = m_stack.top(); m_stack.pop();
    m_stack.emplace(it);
  }

  /****************************************************************************
   * UPDATE statment.
   * Update matching row(s).
   */
  void vm::update () {
    try {
      // get corresponding tree
      auto the_tree = m_tree_list.back()->tree();
      // compact each item in assignment list
      for (auto p : m_assignment_list)
        compact_condition(p.second);
      // parse where condition
//      std::vector<std::pair<std::string, std::string> > col_val_list;
//      parse_where(m_where, col_val_list);
      // make key
//      std::string key;
//      make_update_key(key, the_tree->get_schema(), col_val_list);
      // find matching row(s)
      std::vector<std::string> key_list;
      for (auto iter = the_tree->begin(); iter != the_tree->end(); ++iter) {
        auto a_row = *iter;
        set_column_value(m_where, a_row);
        std::string res;
        exec_condition(res, m_where);
        if (res == "true") {
          std::string key = a_row.make_key(schema::PK_NDX);
          key_list.push_back(key);
        }
      }
      // update matching row(s)
      size_t row_count {0L};
      for (auto key : key_list) {
        // find original-row
        auto iter = the_tree->find(schema::PK_NDX, key);
        if (iter == the_tree->end(schema::PK_NDX)) continue;
        auto org_row = *iter;
        // set and exec assignment
        for (auto p : m_assignment_list) {
          set_column_value(p.second, org_row);
          exec_assignment(p.second->m_left->m_value, p.second);
        }
        // remove orignial-row
        the_tree->remove(org_row);
        // make update-row
        auto upd_row = the_tree->make_row();
        // populate update-row
        auto& column_list = the_tree->get_schema()->get_column_list();
        for (size_t i = 0; i < column_list.size(); ++i)
        {
          auto iter
            = std::find_if(m_assignment_list.begin(), m_assignment_list.end(),
              [&](auto p){return (column_list[i].name() == p.first);});

          if (iter != m_assignment_list.end())
            upd_row.append(i, (*iter).second->m_left->m_value);
          else
            upd_row.append(i, org_row.value_at(i));
        }
        // insert update-row
        the_tree->insert(upd_row);
        ++row_count;
      }
      m_dbms->flush(the_tree);
      // stop time
      auto t1 = Time::now();
      // caculate elapsed time
      fsec fs = t1 - m_t0;
      // output status message
      std::ostringstream oss;
      oss << "Query OK, " << row_count << " rows affected (" << std::fixed
        << std::setw(4) << std::setprecision(2) << fs.count() << " sec)\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   * USE DATABASE statement.
   * Change directory to the corresponding database directory.
   */
  void vm::use_database () {
    try {
      // pop item off stack
      auto e = m_stack.top(); m_stack.pop();
      // get offset of table name
      int32_t offset = e->value();
      // get database name
      std::string db_name = get_varchar(offset + 1);
      // construct path to database
      std::ostringstream path;
      path << m_dbms->path() << '/' << db_name;
      // change directory
      if (chdir(path.str().c_str()) < 0) {
        if (errno == ENOENT) throw std::runtime_error{eno::get_msg(
            eno::ERRNO_MALFORMED_DATABASE_REF, errno)};
        else throw std::runtime_error{eno::get_msg(
            eno::ERRNO_SYSTEM_ERROR, errno)};
      }
      // update database currently in use
      m_fsm->get_info().db_name(db_name);
      // output status message
      std::ostringstream oss;
      oss << "Database changed\n";
      m_fsm->reset();
      m_fsm->put(oss.str());
      fsm_flush();
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::using_cond () {
    // pop item off stack (skip db_name)
    auto e1 = m_stack.top(); m_stack.pop();
    // get column-name-count
    int32_t count = e1->value();
    // process column names
    std::vector<std::string> col_name_list;
    for (int32_t i = 0L; i < count; ++i) {
      // pop item off stack (skip db_name)
      auto e2 = m_stack.top(); m_stack.pop();
      // pop item off stack (skip tbl_name)
      e2 = m_stack.top(); m_stack.pop();
      // pop item off stack
      e2 = m_stack.top(); m_stack.pop();
      // get offset
      int32_t offset = e2->value();
      // get column name
      std::string col_name = get_varchar(offset + 1);
      // push column name onto list
      col_name_list.push_back(col_name);
    }
    for (size_t i = 0L; i < col_name_list.size(); ++i) {
      // get left tree
      auto const l_tree = m_tree_list[m_tbl_index];
      // get right tree
      auto const r_tree = m_tree_list[m_tbl_index - 1L];
      // construct right (column) item
      auto r_item = std::shared_ptr<item>{new item};
      r_item->m_type      = TYPE_COLUMN;
      r_item->m_db_name   = r_tree->tree()->db_name();
      r_item->m_tbl_name  = r_tree->tree()->tbl_name();
      r_item->m_col_name  = col_name_list[ i ];
      r_item->m_parent    = r_tree;
      // push (column) item onto stack
      m_stack.push(r_item);
      // construct right (column) item
      auto l_item = std::shared_ptr<item>{new item};
      l_item->m_type      = TYPE_COLUMN;
      l_item->m_db_name   = l_tree->tree()->db_name();
      l_item->m_tbl_name  = l_tree->tree()->tbl_name();
      l_item->m_col_name  = col_name_list[ i ];
      l_item->m_parent    = l_tree;
      // push (column) item onto stack
      m_stack.push(l_item);
      // make an equal-to comparison item on the stack
      cmpeq();
    }
    // construct condition
    for (int32_t i = 0L; i < count - 1; ++i) logic_and();
    // make an on-condition item on the stack
    on_cond();
  }

  /****************************************************************************
   * Validate the values of foreign-key columns by searching the primary-key
   * index of the foreign table for a match. Return true if a match is found,
   * otherwise return false.
   */
  bool vm::verify_foreign_key (
      std::vector<schema::foreign> const& fgn_list,
      std::vector<std::pair<std::string,
        std::shared_ptr<tree::tree> > > const& fgn_tree_list, row& rw)
  {
    for (auto fgn : fgn_list) {
      // find the foreign table (tree)
      auto iter1 = std::find_if(fgn_tree_list.begin(), fgn_tree_list.end(),
          [&](auto& p){ return (p.first == fgn.name()); });
      auto fgn_tree = (*iter1).second;
      if (fgn_tree == nullptr)
        throw std::runtime_error{eno::get_msg(eno::ERRNO_DBMS_ERROR)};
      // construct a primary-key for foreign table
      auto key = rw.make_foreign_key(fgn.name());
      // search primary-key index of foreign table for a match
      auto iter2 = fgn_tree->find(schema::PK_NDX, key);
      // if no match then invalid row
      if (iter2 == fgn_tree->end(schema::PK_NDX)) return false;
    }
    // valid row
    return true;
  }

  /****************************************************************************
   *
   */
  bool vm::verify_global_privilege (std::string const& col_name)
  {
    try {
      // construct schema-name
      std::ostringstream sch_name;
      sch_name << dbms::ETC_DIR << '.' << dbms::CRED_TBL;
      // get block-cache object
      auto blk_cache = m_dbms->get_block_cache(sch_name.str());
      // construct tree object
      auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
      // unique-lock tree
      the_tree->set_unique_lock();
      // construct row object
      auto rw = the_tree->make_row();
      // populate row
      rw.clear();
      rw.append(0, m_fsm->get_info().hostname());
      rw.append(1, m_fsm->get_info().username());
      // make primary-key
      std::string key = rw.make_key(schema::PK_NDX);
      // find corresponding row, if exists
      auto iter = the_tree->find(schema::PK_NDX, key);
      // if row does not exist
      if (iter == the_tree->end(schema::PK_NDX)) return false;
      // verify privilege
      auto value = (*iter).value_at(col_name);
      if (value == dbms::YES) return true;
      return false;
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  bool vm::verify_local_privilege (std::string const& db_name,
      std::string const& tbl_name, std::string const& col_name)
  {
    try {
      // construct schema-name
      std::ostringstream sch_name;
      sch_name << dbms::ETC_DIR << '.' << dbms::PRIV_TBL;
      // get block-cache object
      auto blk_cache = m_dbms->get_block_cache(sch_name.str());
      // construct tree object
      auto the_tree = std::shared_ptr<tree::tree>{new tree::tree{blk_cache}};
      // unique-lock tree
      the_tree->set_unique_lock();
      // construct row object
      auto rw = the_tree->make_row();
      // construct priv-type list
      std::vector<std::pair<std::string, std::string> > priv_type_list {
        {db_name, tbl_name}, {db_name, dbms::ASTERISK},
          {dbms::ASTERISK, dbms::ASTERISK}
      };
      // find matching row in privilege table
      bool matched {false};
      for (auto p : priv_type_list) {
        // populate row
        rw.clear();
        rw.append(0, m_fsm->get_info().hostname());
        rw.append(1, m_fsm->get_info().username());
        rw.append(2, p.first);
        rw.append(3, p.second);
        // make primary-key
        std::string key = rw.make_key(schema::PK_NDX);
        // find corresponding row, if exists
        auto iter2 = the_tree->find(schema::PK_NDX, key);
        // if row does not exist
        if (iter2 != the_tree->end(schema::PK_NDX)) {
          matched = true;
          rw = *iter2;
          break;
        }
      }
      // no matching row found in privilege table
      if (not matched) return false;
      // verify privilege
      auto value = rw.value_at(col_name);
      if (value == dbms::YES) return true;
      return false;
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  void vm::verify_privilege (std::string const& db_name,
      std::string const& tbl_name, std::string const& col_name)
  {
    if (not verify_global_privilege(col_name)
        && not verify_local_privilege(db_name, tbl_name, col_name))
      throw std::runtime_error{eno::get_msg(
          eno::ERRNO_ACCESS_DENIED)};
  }

  /****************************************************************************
   *
   */
  void vm::verify_privilege (std::string const& db_name,
      std::string const& tbl_name, std::vector<std::string> const& priv_list)
  {
    for (auto col_name : priv_list) {
      if (verify_global_privilege(col_name)
          || verify_local_privilege(db_name, tbl_name, col_name)) return;
    }
    throw std::runtime_error{eno::get_msg(
        eno::ERRNO_ACCESS_DENIED)};
  }

  /****************************************************************************
   *
   */
  bool vm::verify_unique_key (std::shared_ptr<tree::tree> the_tree, row& rw) {
    for (auto ndx : the_tree->get_schema()->get_index_list()) {
      if ((ndx.flag() & schema::FLAG_UNIQUE) == schema::FLAG_UNIQUE) {
        auto key = rw.make_key(ndx.name());
        auto iter = the_tree->find(ndx.name(), key);
        auto end = the_tree->end(ndx.name());
        if (iter != end) return false;
      }
    }
    return true;
  }

  /****************************************************************************
   *
   */
  void vm::verify_value_list (std::shared_ptr<tree::tree> the_tree,
      std::vector<std::pair<std::string, std::string> >& col_val_list)
  {
    auto sch = the_tree->get_schema();
    auto rw = the_tree->make_row();
    for (size_t i = 0; i < sch->get_column_list().size(); ++i) {
      auto iter = std::find_if(col_val_list.begin(), col_val_list.end(),
          [&](auto p){ return (p.first == sch->column_at(i).name()); });
      if (iter != col_val_list.end()) {
        if (!rw.verify_value(i, iter->second))
          throw std::runtime_error{eno::get_msg(
              eno::ERRNO_MALFORMED_VALUE_LIST)};
      } else {
        if (!rw.verify_value(i, ""))
          throw std::runtime_error{eno::get_msg(
              eno::ERRNO_MALFORMED_VALUE_LIST)};
      }
    }
  }

  /****************************************************************************
   *
   */
  void vm::where () {
    m_where = std::shared_ptr<item> {new item};
    m_where->m_type = TYPE_WHERE;
    m_where->m_action = TYPE_WHERE;
    m_where->m_on_cond = m_stack.top(); m_stack.pop();
  }

  /****************************************************************************
   *
   */
  void vm::yield () {
    auto y1 = Time::now();
    // caculate elapsed time
    fsec fs = y1 - m_y0;
    // get elapsed milliseconds
    ms ems = std::chrono::duration_cast<ms>(fs);
    // yield if 500 milliseconds have passed
    if (ems.count() >= 500L) {
      std::this_thread::yield();  // yield
      m_y0 = Time::now(); // update start time
    }
  }

} // namespace


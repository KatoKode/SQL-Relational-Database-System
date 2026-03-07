//-----------------------------------------------------------------------------
// Desc: RedB database installation program
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

#include "main.h"

int main (int argc, char**argv) {

  bool have_set_uid {true};
  
  std::string uid_name;
  if (argc == 2) {
    std::string arg {argv[1]};

    std::string option;
    auto iter = arg.begin();
    for (; iter != arg.end() && *iter != '='; ++iter)
      option += *iter;

    if (option == USER_OPT) {
      if (*iter == '=') ++iter;

      for (; iter != arg.end(); ++iter)
        uid_name += *iter;
    }

    have_set_uid = set_effective_user_id(uid_name.c_str());
  }

  if (not have_set_uid) return -1;

  (void) atexit (exit_handler);

  sigfunc *SIGINT_func = SIG_ERR;
  sigfunc *SIGQUIT_func = SIG_ERR;
  sigfunc *SIGABRT_func = SIG_ERR;
  sigfunc *SIGPIPE_func = SIG_ERR;
  sigfunc *SIGTERM_func = SIG_ERR;

  if ((SIGINT_func = sa_signal (SIGINT, signal_handler)) == SIG_ERR)
    return -1;

  if ((SIGQUIT_func = sa_signal (SIGQUIT, signal_handler)) == SIG_ERR)
    return -1;

  if ((SIGABRT_func = sa_signal (SIGABRT, signal_handler)) == SIG_ERR)
    return -1;

  if ((SIGPIPE_func = sa_signal (SIGPIPE, signal_handler)) == SIG_ERR)
    return -1;

  if ((SIGTERM_func = sa_signal (SIGTERM, signal_handler)) == SIG_ERR)
    return -1;

  try {
    red::eno::init();

    // construct log path
    std::ostringstream log_path;
    log_path << red::dbms::BASE_DIR << '/' << red::dbms::LOG_DIR;

    // construct logger
    logger = std::shared_ptr<red::logger> {
      new red::logger {log_path.str()}
    };

    // construct etc path
    std::ostringstream etc_path;
    etc_path << red::dbms::BASE_DIR << '/' << red::dbms::ETC_DIR;

    // construct schema manager
    schema_mgr = std::shared_ptr<red::schema::manager> {
      new red::schema::manager {etc_path.str()}
    };

    // construct crypto generator
    crypto = std::shared_ptr<red::srvcry> {
      new red::srvcry {logger, etc_path.str(), red::dbms::PKEY_FIL,
        red::dbms::KEYP_FIL, red::dbms::HASH_FIL}
    };

    create_hash_file();
    create_keyp_file();
    create_cred_tbl();
    create_priv_tbl();
    create_root_user();

  } catch(std::runtime_error& e) {
    std::cerr << e.what() << '\n';
    return -1;
  }

  return 0;
}

/******************************************************************************
 *
 */
void exit_handler () {
}

/******************************************************************************
 *
 */
void signal_handler (int signo) {
}

/******************************************************************************
 *
 */
sigfunc* sa_signal (int signo, sigfunc* func)
{
  struct sigaction new_action;
  struct sigaction old_action;

  (void) memset(static_cast<void*>(&new_action), 0, sizeof(new_action));

  new_action.sa_handler = func;

  sigemptyset (&new_action.sa_mask);

  new_action.sa_flags = 0;

  if (signo == SIGALRM) {
#ifdef    SA_INTERRUPT
    new_action.sa_flags |= SA_INTERRUPT;
#endif
  } else {
#ifdef    SA_RESTART
    new_action.sa_flags |= SA_RESTART;
#endif
  }

  if (sigaction (signo, &new_action, &old_action) < 0)
    return (SIG_ERR);

  return old_action.sa_handler;
}

/******************************************************************************
 *
 */
void create_cred_tbl () {
  try {
    auto cred_schema = std::shared_ptr<red::schema::schema>{
      make_cred_schema()};

    schema_mgr->save_schema(cred_schema);

    std::ostringstream path;
    path << red::dbms::BASE_DIR << '/' << red::dbms::ETC_DIR << '/'
      << red::dbms::CRED_TBL << ".d";

    red::dio io {true, path.str()};
  } catch (...) { throw; }
}

/******************************************************************************
 *
 */
void create_hash_file () {
  crypto->hashgen();
  crypto->load_hash();
}

/******************************************************************************
 *
 */
void create_keyp_file () {
  crypto->keygen();
}

/******************************************************************************
 *
 */
void create_priv_tbl () {
  try {
    auto priv_schema = std::shared_ptr<red::schema::schema>{
      make_priv_schema()};

    schema_mgr->save_schema(priv_schema);

    std::ostringstream path;
    path << red::dbms::BASE_DIR << '/' << red::dbms::ETC_DIR << '/'
      << red::dbms::PRIV_TBL << ".d";

    red::dio io {true, path.str()};
  } catch (...) { throw; }
}

/******************************************************************************
 *
 */
void create_root_user () {
  try {
    std::ostringstream sch_name;
    sch_name << red::dbms::ETC_DIR << '.' << red::dbms::CRED_TBL;
    auto sch = schema_mgr->get_schema(sch_name.str());
    if (sch == nullptr)
      throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_SCHEMA_ERROR)};

    std::ostringstream path;
    path << red::dbms::BASE_DIR << '/' << red::dbms::ETC_DIR << '/'
      << red::dbms::CRED_TBL << ".d";
    red::dio io {false, path.str()};

    auto blk_cache = std::shared_ptr<red::block_cache>{
      new red::block_cache{io, sch}};
    if (blk_cache == nullptr)
      throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_SYSTEM_ERROR)};

    auto the_tree = std::shared_ptr<red::tree::tree>{
      new red::tree::tree{blk_cache}};
    if (blk_cache == nullptr)
      throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_SYSTEM_ERROR)};

    std::string hostname {"localhost"};
    std::string username {"root"};
    std::string password;
    if (not crypto->hash_text(password, ""))
      throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_SYSTEM_ERROR)};

    std::string yes = {"Y"};

    auto rw = the_tree->make_row();
    rw.append(0L, hostname);
    rw.append(1L, username);
    rw.append(2L, password);
    for (size_t i = 3L;
        i < static_cast<size_t>(sch->get_header().column_count()); ++i)
      rw.append(i, yes);

    the_tree->insert(rw);

    rw.clear();

    hostname = "127.0.0.1/8";

    rw.append(0L, hostname);
    rw.append(1L, username);
    rw.append(2L, password);
    for (size_t i = 3L;
        i < static_cast<size_t>(sch->get_header().column_count()); ++i)
      rw.append(i, yes);

    the_tree->insert(rw);

    blk_cache->flush_cache();

    schema_mgr->save_schema(sch_name.str());
  } catch (...) { throw; }
}

/******************************************************************************
 *
 */
red::schema::schema* make_cred_schema () {
  int32_t const key_flag {(red::schema::FLAG_NOT_NULL
      | red::schema::FLAG_PRIMARY
      | red::schema::FLAG_UNIQUE)};
  int32_t const col_flag {red::schema::FLAG_NOT_NULL};
  std::vector<std::string> enum_list {"N","Y"};
  std::vector<red::schema::column> column_list {
    {key_flag, 64, red::schema::TYPE_VARCHAR, "hostname"},
    {key_flag, 32, red::schema::TYPE_VARCHAR, "username"},
    {col_flag, 256, red::schema::TYPE_VARCHAR, "password"},
    {col_flag, 1, red::schema::TYPE_ENUM, "select_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "insert_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "update_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "delete_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "index_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "alter_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "create_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "drop_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "grant_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "reload_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "shutdown_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "file_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "show_db_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "create_user_priv", enum_list},
  };

  int32_t const key_size = column_list[0].size() + column_list[1].size();

  red::tree::bcalc bc {key_size};

  std::vector<std::string> col_name_list {
    column_list[0].name(), column_list[1].name()
  };

  std::vector<red::schema::index> index_list {
    {(red::schema::FLAG_NOT_NULL | red::schema::FLAG_PRIMARY
        | red::schema::FLAG_UNIQUE), bc.order(), key_size,
      red::schema::PK_NDX, col_name_list}
  };

  red::schema::header header {
    red::dbms::ETC_DIR, red::dbms::CRED_TBL, 0, red::tree::BUFFER_SZ,
      static_cast<int32_t>(index_list.size()), 0,
      static_cast<int32_t>(column_list.size()), 0L, -1L, -1L
  };

  return new red::schema::schema {header, column_list, index_list};
}

/******************************************************************************
 *
 */
red::schema::schema* make_priv_schema () {
  int32_t const key_flag {(red::schema::FLAG_NOT_NULL
      | red::schema::FLAG_PRIMARY
      | red::schema::FLAG_UNIQUE)};
  int32_t const col_flag {red::schema::FLAG_NOT_NULL};
  std::vector<std::string> enum_list {"N","Y"};
  std::vector<red::schema::column> column_list {
    {key_flag, 64, red::schema::TYPE_VARCHAR, "hostname"},
    {key_flag, 32, red::schema::TYPE_VARCHAR, "username"},
    {key_flag, 32, red::schema::TYPE_VARCHAR, "db_name"},
    {key_flag, 32, red::schema::TYPE_VARCHAR, "tbl_name"},
    {col_flag, 1, red::schema::TYPE_ENUM, "select_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "insert_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "update_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "delete_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "index_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "alter_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "create_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "drop_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "grant_priv", enum_list},
    {col_flag, 1, red::schema::TYPE_ENUM, "file_priv", enum_list},
  };

  int32_t const key_size = column_list[0].size() + column_list[1].size()
    + column_list[2].size() + column_list[3].size();

  red::tree::bcalc bc {key_size};

  std::vector<std::string> col_name_list {
    column_list[0].name(), column_list[1].name(),
    column_list[2].name(), column_list[3].name()
  };

  std::vector<red::schema::index> index_list {
    {(red::schema::FLAG_NOT_NULL | red::schema::FLAG_PRIMARY
        | red::schema::FLAG_UNIQUE), bc.order(), key_size,
      red::schema::PK_NDX, col_name_list}
  };

  red::schema::header header {
    red::dbms::ETC_DIR, red::dbms::PRIV_TBL, 0, red::tree::BUFFER_SZ,
      static_cast<int32_t>(index_list.size()), 0,
      static_cast<int32_t>(column_list.size()), 0L, -1L, -1L
  };

  return new red::schema::schema {header, column_list, index_list};
}

/******************************************************************************
 *
 */
bool set_effective_user_id (char const* name) {
  struct passwd* pb {nullptr};
  if ((pb = getpwnam(name)) == nullptr) return false;
  if (setuid(pb->pw_uid) != 0) return false;
  return true;
}


//-----------------------------------------------------------------------------
// Desc: RedB server program
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

/******************************************************************************
 *
 */
int main (int argc, char**argv) {

  (void) atexit (exit_handler);

  sigset_t nmask;
  sigset_t omask;
  sigfunc *SIGTERM_func = SIG_ERR;
  init_signal(&nmask, &omask, SIGTERM_func);

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
  } else have_set_uid = false;

  if (not have_set_uid) return -1;

  try {
    // initialize error class
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

    // construct dbms
    dbms = std::shared_ptr<red::dbms::dbms> {
      new red::dbms::dbms {schema_mgr, logger, decrypt_stmt}
    };

    // construct crypto
    crypto = std::shared_ptr<red::srvcry> {
      new red::srvcry{logger, etc_path.str(), red::dbms::PKEY_FIL,
      red::dbms::KEYP_FIL, red::dbms::HASH_FIL}
    };
    get_hashcode();
    get_keypair();

    std::thread comm_thrd {comm_server};

    while (true) {
      std::this_thread::sleep_for(std::chrono::seconds(2));

      {
        std::unique_lock<std::mutex> lck {srv_mutex};
        if (not all_systems_go) break;
        if (not comm_up) comm_thrd = std::thread {comm_server};
      }
    }

    {
      std::unique_lock<std::mutex> lck {cli_mutex};
      for (auto p : cli_list) {
        p.second->reset(red::comm::OP_TERM_SYN);
        p.second->flush();
        p.first->join();
      }
    }

    red::comm::conn conn;
    conn.cli_init(LOCALHOST.c_str(), SRVRPORT.c_str());
    comm_thrd.join();

    dbms->kill();
  } catch(std::exception& e) {
    logger->write(red::CRITICAL, e.what());
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
void init_signal (sigset_t* nmask, sigset_t* omask, sigfunc *SIGTERM_func) {
  sigemptyset(nmask);
  sigaddset(nmask, SIGINT);
  sigaddset(nmask, SIGQUIT);
  sigaddset(nmask, SIGABRT);
  sigaddset(nmask, SIGPIPE);
  (void) sigprocmask(SIG_BLOCK, nmask, omask);

  SIGTERM_func = sa_signal (SIGTERM, signal_handler);
}

/******************************************************************************
 *
 */
void signal_handler (int signo) {
  if (signo == SIGTERM) {
    std::unique_lock<std::mutex> lck {srv_mutex};
    all_systems_go = false;
  }
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
std::string decrypt_stmt (size_t buflen, uint8_t* buffer) {
  uint8_t* buf = crypto->decrypt(buflen, buffer);
  std::string stmt = reinterpret_cast<char*>(buf);
  return stmt;
}

/******************************************************************************
 *
 */
bool authenticate (std::shared_ptr<red::comm::fsm> cli_fsm,
    std::string const& username, std::string const& password)
{
  try {
    std::ostringstream bc_name;
    bc_name << red::dbms::ETC_DIR << '.' << red::dbms::CRED_TBL;

    auto blk_cache = dbms->get_block_cache(bc_name.str());

    if (blk_cache == nullptr)
      blk_cache = dbms->load_block_cache(bc_name.str());

    red::tree::tree the_tree{blk_cache};

    the_tree.set_unique_lock();

    std::string hashcode;
    if (not crypto->hash_text(hashcode, password)) return false;

    auto end = the_tree.end();
    for (auto iter = the_tree.begin(); iter != end; ++iter)
    {
      auto rw = *iter;

      if (cli_fsm->in_subnet(rw.value_at(red::dbms::HOST_FLD))
          && username == rw.value_at(red::dbms::USER_FLD)
          && hashcode == rw.value_at(red::dbms::PWRD_FLD))
      {
        red::comm::info a_info {rw.value_at(red::dbms::HOST_FLD), username};
        cli_fsm->set_info(a_info);
        for (char& ch : hashcode) ch = ' ';
        return true;
      }
    }

    for (char& ch : hashcode) ch = ' ';

  } catch (std::runtime_error& e) {
    std::ostringstream oss;
    oss << e.what() << '\n';
    cli_fsm->reset(red::comm::OP_AUTH_ERR);
    cli_fsm->put(oss.str());
    cli_fsm->flush();
  } catch (std::exception& e) { logger->write(red::CRITICAL, e.what()); }

  return false;
}

/******************************************************************************
 *
 */
bool exec_source (std::shared_ptr<red::comm::fsm> cli_fsm,
    std::string const& src_stmt)
{
  try {
    std::smatch sm;
    if (not std::regex_search(src_stmt, sm, SOURCE_CMD)) return false;
    std::ostringstream path;
    path << red::dbms::SRC_DIR;
    if (sm.str(3).front() != '/') path << '/';
    path << sm.str(3);
    std::ifstream ifs {path.str()};
    if (not ifs.is_open())
      throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_SOURCE_ERROR)};
    std::vector<std::string> stmt_list;
    while (true) {
      std::string stmt;
      std::getline(ifs, stmt);
      if (not ifs) break;
      if (not stmt.empty()) stmt_list.push_back(stmt);
    }
    ifs.clear();
    ifs.close();
    size_t size {stmt_list.size()};
    for (auto stmt : stmt_list) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      int32_t opcode {red::comm::OP_RSLT_SYN};
      if (--size == 0) opcode = red::comm::OP_NULL;
      red::sql a_sql {stmt};
      red::vm{opcode, a_sql.compile_stmt(), dbms, cli_fsm, crypto, logger}.run();
//      a_vm.run();
    }
  } catch (std::runtime_error& e) {
    report_error(cli_fsm, e, red::comm::OP_STMT_ERR);
  } catch (std::exception& e) { logger->write(red::ERROR, e.what()); }

  return true;
}

/******************************************************************************
 *
 */
std::string get_encrypted_stmt (size_t encLength, uint8_t* encBuffer) {
  std::string stmt = decrypt_stmt(encLength, encBuffer);
  auto iter = std::find(stmt.begin(), stmt.end(), ';');
  ++iter;
  stmt.erase(iter, stmt.end());
  return stmt;
}

/******************************************************************************
 *
 */
bool get_hashcode () {
  return crypto->load_hash();
}

/******************************************************************************
 *
 */
bool get_keypair () {
  return crypto->load_keypair();
}

/******************************************************************************
 *
 */
void rcvd (std::shared_ptr<red::comm::fsm> cli_fsm) {
  auto opcode = cli_fsm->get_opcode();
  switch (opcode) {
    case red::comm::OP_TERM_SYN:
      try {
        cli_fsm->reset(red::comm::OP_TERM_ACK);
        cli_fsm->flush();
        cli_fsm->term();
      } catch (std::exception& e) { logger->write(red::ERROR, e.what()); }
      break;
    case red::comm::OP_TERM_ACK:
      cli_fsm->term();
      break;
    case red::comm::OP_RSLT_ACK:
      cli_fsm->term();
      break;
    case red::comm::OP_CONN_SYN:
      {
        cli_fsm->reset(red::comm::OP_CONN_ACK, red::comm::TYPE_TXT);
        size_t buflen = cli_fsm->remaining();
        crypto->pubkey_to_buffer(&buflen, cli_fsm->begin());
        if (buflen > red::comm::OPCODE_SZ)
          cli_fsm->size(cli_fsm->size() + buflen);
        cli_fsm->flush();
      }
      break;
    case red::comm::OP_AUTH_SYN:
      try {
        std::string cred = decrypt_stmt(cli_fsm->size() - 2L,
            cli_fsm->begin());
        std::string username;
        std::string password;
        size_t i {0};
        for (; i < cred.size() && cred[i] != ' '; ++i) username += cred[i];
        for (++i; i < cred.size() && cred[i] != ' '; ++i) password += cred[i];
        for (char& ch : cred) ch = ' ';
        if (authenticate(cli_fsm, username, password)) {
          cli_fsm->reset(red::comm::OP_AUTH_ACK);
          cli_fsm->flush();
        } else {
          cli_fsm->reset(red::comm::OP_AUTH_ERR);
          cli_fsm->flush();
          cli_fsm->term();
        }
        for (char& ch : password) ch = ' ';
        for (char& ch : username) ch = ' ';
      } catch (std::runtime_error& e) {
        report_error(cli_fsm, e, red::comm::OP_AUTH_ERR);
      } catch (std::exception& e) { logger->write(red::ERROR, e.what()); }
      break;
    case red::comm::OP_STMT_SYN:
      try {
        std::string stmt;
        if (cli_fsm->get_type() == red::comm::TYPE_ENC) {
          stmt = get_encrypted_stmt(cli_fsm->size() - 2L,
              &cli_fsm->buffer()[2]);
        } else {
          auto end = cli_fsm->end();
          for (auto iter = cli_fsm->begin(); iter != end; ++iter)
            stmt += *iter;
        }
        if (not exec_source(cli_fsm, stmt)) {
          red::sql a_sql {stmt};
          red::vm {a_sql.compile_stmt(), dbms, cli_fsm, crypto, logger}.run();
        }
      } catch (std::runtime_error& e) {
        report_error(cli_fsm, e, red::comm::OP_STMT_ERR);
      } catch (std::exception& e) { logger->write(red::ERROR, e.what()); }
      break;
    default:;
  }
}

/******************************************************************************
 *
 */
void report_error (std::shared_ptr<red::comm::fsm> cli_fsm, std::runtime_error& e,
    int32_t const opcode)
{
  std::ostringstream oss;
  oss << e.what() << '\n';
  cli_fsm->reset(opcode);
  cli_fsm->put(oss.str());
  cli_fsm->flush();
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

/******************************************************************************
 *
 */
void comm_agent (std::shared_ptr<red::comm::fsm> cli_fsm) {
  try {
    cli_fsm->loop(cli_fsm);
  } catch (std::exception& e) { logger->write(red::CRITICAL, e.what()); }
}

/******************************************************************************
 *
 */
void comm_server () {
  try {
    red::comm::conn srv_conn;

    srv_conn.srv_init(nullptr, SRVRPORT.c_str(), ACCEPT_BACKLOG);

    auto rcvd_cb = std::bind(rcvd, std::placeholders::_1);

    while (true) {
      auto new_conn = srv_conn.accept_conn();

      auto cli_fsm = std::shared_ptr<red::comm::fsm> {
        new red::comm::fsm {new_conn, rcvd_cb}};

      auto thrd = std::shared_ptr<std::thread> {
        new std::thread {comm_agent, cli_fsm}};

      {
        std::unique_lock<std::mutex> lck {cli_mutex};
        cli_list.push_back(std::make_pair(thrd, cli_fsm));
      }

      {
        std::unique_lock<std::mutex> lck {srv_mutex};
        if (not all_systems_go) break;
      }
    }
  } catch (std::exception& e) {
    logger->write(red::CRITICAL, e.what());
    std::unique_lock<std::mutex> lck {srv_mutex};
    comm_up = false;
  }
}


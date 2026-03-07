//-----------------------------------------------------------------------------
// Desc: RedB client program
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

int main (int argc, char** argv) {

  (void) atexit (exit_handler);

  sigset_t nmask;
  sigset_t omask;
  sigfunc *SIGTERM_func = SIG_ERR;
  init_signal(&nmask, &omask, SIGTERM_func);

  if (argc < 3) return 0; // required: redcli -u <username>

  if (strcmp(argv[1], "-u") != 0) return 0;

  std::string username {argv[2]};
  std::string password;

  if (argc == 4) {  // optional: -p
    if (strcmp(argv[3], "-p") != 0) return 0;
    password = getpass();
  }

  std::cout << LICENSE << "\n\n";

  using_history ();

  try {
    auto conn = std::shared_ptr<red::comm::conn> {new red::comm::conn};
    conn->cli_init(SRVRHOST.c_str(), SRVRPORT.c_str());

    auto cli_fsm = std::shared_ptr<red::comm::fsm>{
      new red::comm::fsm{conn, rcvd}
    };

    cli_fsm->reset(red::comm::OP_CONN_SYN, red::comm::TYPE_TXT);
    cli_fsm->flush();
    cli_fsm->loop(cli_fsm);

    if (not conn_ok) return -1;

    {
      cli_fsm->reset(red::comm::OP_AUTH_SYN, red::comm::TYPE_ENC);
      size_t buflen = cli_fsm->remaining();
      if (not encrypt_cred(username, password, &buflen, cli_fsm->begin()))
        return -1;
      cli_fsm->size(cli_fsm->size() + buflen);
      cli_fsm->flush();
      cli_fsm->cont();
      cli_fsm->loop(cli_fsm);

      if (not auth_ok) return -1;
    }

    std::thread cli_thrd {cli_agent, cli_fsm};

    if (have_history()) load_history();

    std::string line;
    do {
      std::ostringstream oss;
      oss << prompt << brckt;
      char* input;
      if ((input = readline(oss.str().c_str())) != nullptr) {
        line += input;
        free(input);
      }

      if (do_exit()) break;

      if (line == EXIT_STMT || line == QUIT_STMT) {
        push_history(line);
        cli_fsm->reset(red::comm::OP_TERM_SYN);
        set_done(false);
        cli_fsm->flush();
        line.clear();
      } else if (not line.empty() && line.back() != ';') {
        brckt = cont_brckt;
      } else if (not line.empty()) {
        push_history(line);
        std::vector<std::string> stmt_list;
        parse_line(stmt_list, line);
        for (auto stmt : stmt_list) {
          cli_fsm->reset(red::comm::OP_STMT_SYN, red::comm::TYPE_TXT);
          if (do_encrypt(stmt)) {
            cli_fsm->set_type(red::comm::TYPE_ENC);
            size_t buflen = cli_fsm->remaining();
            if (not encrypt_stmt(stmt, &buflen, cli_fsm->begin())) break;
            cli_fsm->size(cli_fsm->size() + buflen);
          } else cli_fsm->put(stmt);
          set_done(false);
          cli_fsm->flush();
        }
        line.clear();
        brckt = norm_brckt;
      }

      wait_done();

      if (do_exit()) break;

    } while (true);

    save_history();

    cli_thrd.join();
  }
  catch (std::runtime_error& e) { std::cerr << e.what() << '\n'; }
  catch (...) { std::cerr << UNKNOWN_ERROR_MSG << '\n'; }

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
  all_systems_go = false;
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
void cli_agent (std::shared_ptr<red::comm::fsm> cli_fsm) {
  try {
    cli_fsm->cont();
    cli_fsm->loop(cli_fsm);
    set_done(true);
    set_exit();
  } catch (std::exception& e) { std::cerr << e.what() << '\n'; }
}

/******************************************************************************
 *
 */
bool do_encrypt (std::string const& stmt) {
  for (auto& rgx : regex_list) {
    std::smatch sm;
    if (std::regex_search(stmt, sm, rgx))
      return (sm.position() == 0L);
  }
  return false;
}

/******************************************************************************
 *
 */
bool do_exit() {
  std::unique_lock<std::mutex>{mtx};
  return not all_systems_go;
}

/******************************************************************************
 * http://www.cplusplus.com/articles/E6vU7k9E/
 */
int get_ch () {
  int ch;
  struct termios t_old, t_new;

  tcgetattr(STDIN_FILENO, &t_old);
  t_new = t_old;
  t_new.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
  return ch;
}

/******************************************************************************
 * http://www.cplusplus.com/articles/E6vU7k9E/ (Modified)
 */
std::string getpass () {
  const char BS=127;
  const char RET=10;

  std::string password;
  unsigned char ch=0;

  std::cout << pw_prompt;

  while((ch=get_ch())!=RET)
  {
    if(ch==BS)
    {
      if(password.length()!=0L)
      {
        std::cout <<"\b \b";
        password.resize(password.length()-1L);
      }
    }
    else
    {
      password+=ch;
      std::cout <<'*';
    }
  }
  std::cout << '\n';
  return password;
}

/******************************************************************************
 *
 */
bool encrypt_cred (std::string& username, std::string& password,
    size_t* outlen, uint8_t* outbuf)
{
  std::ostringstream inbuf;
  inbuf << username << ' ' << password << ' ';
  for (char& ch : username) ch = ' ';
  for (char& ch : password) ch = ' ';
  return encrypt_stmt(inbuf.str(), outlen, outbuf);
}

/******************************************************************************
 *
 */
bool encrypt_stmt (std::string const& stmt, size_t* buflen, uint8_t* buffer) {
  return crypto->encrypt(stmt.size(),
      reinterpret_cast<uint8_t*>(const_cast<char*>(stmt.c_str())),
      buflen, buffer);
}

/******************************************************************************
 *
 */
std::string hash_password (std::string const& password) {
  gcry_error_t     err;
  gcry_md_hd_t hash_hd;

  std::string text_buffer = hashcode;

  for (size_t i = 0; i < password.size(); ++i)
    text_buffer[ i ] = password[ i ];

  if ((err = gcry_md_open(&hash_hd, GCRY_MD_SHA256, GCRY_MD_FLAG_SECURE))
      != GPG_ERR_NO_ERROR)
    throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_CIPHER_ERROR)};

  gcry_md_write(hash_hd, text_buffer.c_str(), text_buffer.size());

  unsigned char *encrypted_buffer;
  if ((encrypted_buffer = gcry_md_read(hash_hd, GCRY_MD_SHA256)) == nullptr)
  {
    gcry_md_close(hash_hd);
    throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_CIPHER_ERROR)};
  }

  std::string encrypted_text;

  uint32_t md_len = gcry_md_get_algo_dlen(GCRY_MD_SHA256);
  for (uint32_t i = 0; i < md_len; i++)
  {
    char ch [8];
    sprintf(ch, "%02X", static_cast<uint8_t>(encrypted_buffer[ i ]));
    encrypted_text += ch;
  }

  gcry_md_close(hash_hd);

  return encrypted_text;
}

/******************************************************************************
 *
 */
bool have_history () {
  try {
    char const* home_dir;
    if ((home_dir = getenv(HOME_ENV.c_str())) == nullptr)
      throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_HOME_VAR)};

    std::ostringstream path;
    path << home_dir << '/' << HIST_FILE;

    errno = 0;
    struct stat sb;
    if (stat(path.str().c_str(), &sb) < 0 || not S_ISREG(sb.st_mode))
      return false;

    return true;
  } catch (...) { throw; }
}

/******************************************************************************
 *
 */
void load_history () {
  try {
    char const* home_dir;
    if ((home_dir = getenv(HOME_ENV.c_str())) == nullptr)
      throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_HOME_VAR)};

    std::ostringstream path;
    path << home_dir << '/' << HIST_FILE;

    if (read_history(path.str().c_str()) != 0)
      throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_WRITE_HISTORY)};
  } catch (...) { throw; }
}

/******************************************************************************
 *
 */
void parse_line (std::vector<std::string>& stmt_list,
    std::string const& raw_stmt)
{
  std::string stmt;
  for (auto iter = raw_stmt.begin(); iter != raw_stmt.end(); ++iter) {
    stmt += *iter;
    if (*iter == ';') {
      if (stmt != SEMICOLON) {
        stmt_list.push_back(stmt);
      }
      stmt.clear();
    }
  }
}

/******************************************************************************
 *
 */
void push_history (std::string const& line) {
  int pos;
  if ((pos = history_search_pos(line.c_str(), 1, 0)) < 0)
    add_history(line.c_str());
  else {
    auto hent = remove_history(pos);
    auto hdat = free_history_entry(hent);
    if (hdat) free(hdat);
    add_history(line.c_str());
  }
}

/******************************************************************************
 *
 */
void rcvd (std::shared_ptr<red::comm::fsm> cli_fsm) {
  auto opcode = cli_fsm->get_opcode();
  switch (opcode) {
    case red::comm::OP_TERM_SYN:
      cli_fsm->reset(red::comm::OP_TERM_ACK);
      cli_fsm->flush();
      cli_fsm->term();
      all_systems_go = false;
      set_done(true);
      break;
    case red::comm::OP_TERM_ACK:
      cli_fsm->term();
      all_systems_go = false;
      set_done(true);
      break;
    case red::comm::OP_RSLT_SYN:
      *cli_fsm->end() = '\0';
      std::cout << cli_fsm->begin();
      std::cout.flush();
      cli_fsm->reset(red::comm::OP_RSLT_ACK, red::comm::TYPE_TXT);
      cli_fsm->flush();
      break;
    case red::comm::OP_RSLT_FIN:
      *cli_fsm->end() = '\0';
      std::cout << cli_fsm->begin() << '\n';
      set_done(true);
      break;
    case red::comm::OP_CONN_ACK:
      *cli_fsm->end() = '\0';
      crypto = std::shared_ptr<red::clicry> {
        new red::clicry{cli_fsm->size_of_data(), cli_fsm->begin()}
      };
      conn_ok = true;
      cli_fsm->term();
      break;
    case red::comm::OP_CONN_ERR:
      conn_ok = false;
      cli_fsm->term();
      break;
    case red::comm::OP_AUTH_ACK:
      auth_ok = true;
      cli_fsm->term();
      break;
    case red::comm::OP_AUTH_ERR:
      auth_ok = false;
      cli_fsm->term();
      break;
    case red::comm::OP_STMT_ERR:
      *cli_fsm->end() = '\0';
      std::cout << cli_fsm->begin();
      set_done(true);
      break;
    default:;
  }
}

/******************************************************************************
 *
 */
void save_history () {
  try {
    char const* home_dir;
    if ((home_dir = getenv(HOME_ENV.c_str())) == nullptr)
      throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_HOME_VAR)};

    std::ostringstream path;
    path << home_dir << '/' << HIST_FILE;

    if (write_history(path.str().c_str()) != 0)
      throw std::runtime_error{red::eno::get_msg(red::eno::ERRNO_READ_HISTORY)};
  } catch (...) { throw; }
}

/******************************************************************************
 *
 */
void set_done (bool b) {
  {
    std::unique_lock<std::mutex>{mtx};
    done = b;
  }
  cond.notify_one();
}

/******************************************************************************
 *
 */
void set_exit () {
  {
    std::unique_lock<std::mutex>{mtx};
    all_systems_go = false;
  }
}

/******************************************************************************
 *
 */
void wait_done () {
  std::unique_lock<std::mutex> lck{mtx};
  cond.wait(lck, [&]{return done;});
}

